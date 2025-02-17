#pragma once
#include <fmt/format.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string_view>

#include "httpxx/configuration.hh"
#include "httpxx/endpoint.hh"
#include "httpxx/objects.hh"
#include "httpxx/router.hh"

namespace httpxx {

class HttpUtils {
 public:
  static float extractHttpVersion(std::string_view http_version) {
    std::string version;
    std::copy_if(http_version.begin(), http_version.end(),
                 std::back_inserter(version),
                 [](char c) { return std::isdigit(c) || c == '.'; });
    return std::stof(version);
  }

  static std::vector<std::string> splitString(std::string_view str,
                                              std::string_view delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string_view::npos) {
      tokens.emplace_back(str.substr(start, end - start));
      start = end + delimiter.length();
      end = str.find(delimiter, start);
    }
    tokens.emplace_back(str.substr(start));

    for (auto& token : tokens) {
      auto it = std::ranges::find_if(token, [](const unsigned char ch) {
        return !std::isspace(ch);
      });
      token.erase(token.begin(), it);
    }

    return tokens;
  }
};

class RequestParser {
 public:
  static Request parse(std::string_view request_string) {
    std::istringstream iss((std::string(request_string)));
    return parseRequestLine(iss);
  }

 private:
  static Request parseRequestLine(std::istringstream& iss) {
    std::string line;
    std::getline(iss, line);

    if (line.find("HTTP") == std::string::npos) {
      throw std::runtime_error("Invalid HTTP request");
    }

    auto parts = HttpUtils::splitString(line, " ");
    if (parts.size() != 3) {
      throw std::runtime_error("Invalid request line format");
    }

    Request request;
    request.method = stringToHttpMethod(parts[0]);
    parseUri(parts[1], request);

    parseHeaders(iss, request);
    parseBody(iss, request);

    return request;
  }

  static void parseUri(std::string_view uri, Request& request) {
    auto questionMark = uri.find('?');
    if (questionMark == std::string_view::npos) {
      request.uri = std::string(uri);
      return;
    }

    request.uri = std::string(uri.substr(0, questionMark));
    auto params = HttpUtils::splitString(uri.substr(questionMark + 1), "&");

    for (const auto& param : params) {
      auto kvPair = HttpUtils::splitString(param, "=");
      if (kvPair.size() == 2) {
        request.request_parameters[kvPair[0]] = kvPair[1];
      }
    }
  }

  static void parseHeaders(std::istringstream& iss, Request& request) {
    std::string line;
    while (std::getline(iss, line) && !line.empty() && line != "\r") {
      auto parts = HttpUtils::splitString(line, ":");
      if (parts.size() == 2) {
        request.headers[parts[0]] = parts[1];
      }
    }
  }

  static void parseBody(std::istringstream& iss, Request& request) {
    std::string body;
    if (std::getline(iss, body)) {
      request.body = body;
    }
  }
};

class ResponseWriter {
 public:
  static void write(const Response& response, int client_fd) {
    writeHeaders(response, client_fd);
    writeBody(response, client_fd);
  }

 private:
  static void writeHeaders(const Response& response, int client_fd) {
    const std::string headers = response.toString();
    ::write(client_fd, headers.c_str(), headers.length());
  }

  static void writeBody(const Response& response, int client_fd) {
    std::visit(overload{[](std::monostate) {},
                        [client_fd](const std::string& str) {
                          ::write(client_fd, str.c_str(), str.length());
                        },
                        [client_fd](const std::vector<char>& vec) {
                          ::write(client_fd, vec.data(), vec.size());
                        }},
               response.body);
  }
};

class FileServer {
 public:
  static Response serveFile(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
      return createErrorResponse(StatusCodes::NOT_FOUND,
                                 "404 - File Not Found");
    }

    try {
      ContentType contentType = getContentTypeFromFilename(path);
      return isTextFile(contentType) ? serveTextFile(path, contentType)
                                     : serveBinaryFile(path, contentType);
    } catch (const std::exception& e) {
      std::clog << "File serving error: " << e.what() << '\n';
      return createErrorResponse(StatusCodes::INTERNAL_SERVER_ERROR,
                                 "500 - Internal Server Error");
    }
  }

 private:
  static Response serveTextFile(const std::filesystem::path& path,
                                ContentType contentType) {
    std::ifstream ifs(path);
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());

    return ResponseBuilder::ok()
        .contentType(contentTypeToString(contentType))
        .body(content)
        .build();
  }

  static Response serveBinaryFile(const std::filesystem::path& path,
                                  ContentType contentType) {
    std::ifstream ifs(path, std::ios::binary);
    std::vector<char> buffer{std::istreambuf_iterator<char>(ifs),
                             std::istreambuf_iterator<char>()};

    return ResponseBuilder::ok()
        .contentType(contentTypeToString(contentType))
        .body(buffer)
        .build();
  }

  static Response createErrorResponse(StatusCodes status,
                                      const std::string& message) {
    return ResponseBuilder()
        .status(status)
        .contentType("text/html")
        .body(fmt::format("<h1>{}</h1>", message))
        .build();
  }
};

class RequestHandler {
 public:
  static void handle(const Router& router, const Config& config, int client_fd,
                     std::string_view buffer) {
    try {
      auto request = RequestParser::parse(buffer);
      auto response = handleRequest(router, config, request);
      ResponseWriter::write(response, client_fd);
    } catch (const std::exception& e) {
      handleError(e, client_fd);
    }
    close(client_fd);
  }

 private:
  static Response handleRequest(const Router& router, const Config& config,
                                const Request& request) {
    if (request.requestsFile()) {
      return FileServer::serveFile(
          fmt::format("{}{}", config.getWwwPath().string(), request.uri));
    }

    auto endpoint = router.get_endpoint(request.uri);
    if (!isMethodAllowed(endpoint, request.method)) {
      return createMethodNotAllowedResponse(request);
    }

    return endpoint.handler(request);
  }

  static bool isMethodAllowed(const Endpoint& endpoint, HttpMethod method) {
    return std::ranges::find(endpoint.accepted_methods, method) !=
           std::end(endpoint.accepted_methods);
  }

  static Response createMethodNotAllowedResponse(const Request& request) {
    auto error =
        fmt::format(R"({{"error": "Method {} is not allowed on URI {}"}})",
                    +request.method, request.uri);

    return ResponseBuilder()
        .status(StatusCodes::METHOD_NOT_ALLOWED)
        .json(nlohmann::json::parse(error))
        .build();
  }

  static void handleError(const std::exception& e, int client_fd) {
    std::clog << e.what() << '\n';
    ResponseWriter::write(
        ResponseBuilder().status(StatusCodes::INTERNAL_SERVER_ERROR).build(),
        client_fd);
  }
};
}
