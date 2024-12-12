#include "http_handlers.hh"

#include <unistd.h>

#include <cstring>
#include <filesystem>
#include <sstream>

#include "http_builders.hh"
#include "http_config.hh"
#include "http_headers.hh"
#include "http_objects.hh"

namespace httpxx {
inline float extractHttpVersion(const std::string& http_version) {
  std::string r{};
  float res{};

  for (const auto& c : http_version) {
    if (std::isdigit(c) || c == '.') {
      r += c;
    }
  }

  res = std::stof(r);

  return res;
}

inline std::vector<std::string> split(const std::string& str,
                                      const std::string& delimiter) {
  auto s = str;
  std::vector<std::string> tokens;
  size_t pos = 0;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    std::string token = s.substr(0, pos);
    token.erase(token.begin(),
                std::ranges::find_if(token, [](const unsigned char ch) {
                  return !std::isspace(ch);
                }));
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);

  return tokens;
}

namespace handlers {
inline Request createRequest(const char* _request_string) {
  RequestLine rl{};

  std::istringstream iss(_request_string);
  std::string line{};
  std::getline(iss, line);
  assert(line.find("HTTP") != std::string::npos);

  std::istringstream intrss(line);
  std::vector<std::string> headers;
  while (std::getline(intrss, line, ' ')) {
    headers.push_back(line);
  }
  assert(headers.size() == 3);

  rl.method = stringToHttpMethod(headers.at(0));
  auto uri = headers.at(1);
  rl.uri = uri;
  rl.http_version = extractHttpVersion(headers.at(2));

  Headers hd{httpxx::headers};
  while (std::getline(iss, line)) {
    // Trim whitespace and check for empty line or just CR/LF
    if (line.empty() || line == "\r") {
      break;
    }

    std::istringstream hss(line);

    auto hv = split(line, ":");
    const auto& head = hv.at(0);
    auto value = hv.at(1);
    assert(not value.empty());

    if (hd.contains(head)) {
      hd.at(head) = std::make_optional(value);
    } else {
      continue;
    }
  }

  std::getline(iss, line);
  Body body{line};

  auto rq = Request{rl, hd, std::make_optional(body)};
  return rq;
}

void response_write(const Response& response, const int client_fd) {
  // Write headers
  const std::string headers = response.toString();
  write(client_fd, headers.c_str(), headers.length());

  // Write body
  std::visit(overload{[](std::monostate) {
                      },
                      [client_fd](const std::string& str) {
                        write(client_fd, str.c_str(), str.length());
                      },
                      [client_fd](const std::vector<char>& vec) {
                        write(client_fd, vec.data(), vec.size());
                      }},
             response.response_body);
}

auto serve_file(const std::filesystem::path& path) -> Response {
  ContentType content_type = getContentTypeFromFilename(path);
  if (!std::filesystem::exists(path)) {
    return http::ResponseBuilder{}
           .setStatusCode(StatusCodes::NOT_FOUND)
           .setContentType(ContentType::TEXT_HTML)
           .setBody("<h1>404 - File Not Found</h1>")
           .build();
  }

  try {
    if (isTextFile(content_type)) {
      std::ifstream ifs(path);
      std::string content((std::istreambuf_iterator<char>(ifs)),
                          std::istreambuf_iterator<char>());

      return http::ResponseBuilder{}
             .setStatusCode(StatusCodes::OK)
             .setContentType(content_type)
             .setBody(content)
             .build();
    } else {
      // Use binary mode for non-text files
      std::ifstream ifs(path, std::ios::binary);
      std::vector<char> buffer{std::istreambuf_iterator<char>(ifs),
                               std::istreambuf_iterator<char>()};

      return http::ResponseBuilder{}
             .setStatusCode(StatusCodes::OK)
             .setContentType(content_type)
             .setBinaryBody(buffer)
             .build();
    }
  } catch (const std::exception& e) {
    std::clog << "File serving error: " << e.what() << '\n';
    return http::ResponseBuilder{}
           .setStatusCode(StatusCodes::INTERNAL_SERVER_ERROR)
           .setContentType(ContentType::TEXT_HTML)
           .setBody("<h1>500 - Internal Server Error</h1>")
           .build();
  }
}

void handle_request(const httpxx::Router& router, const Config& config,
                    int client_fd, const char* buffer) {
  try {
    const auto req = createRequest(buffer);
    Response res{};
    if (req.requestsFile()) {
      res = serve_file(
          std::format("{}{}", config._www_path, req.request_line.uri));
    } else {
      res = router.get_handler_fn(req.request_line.uri)(req);
    }
    response_write(res, client_fd);
    close(client_fd);
  } catch (const std::exception& e) {
    std::clog << e.what() << '\n';
    response_write(http::ResponseBuilder{}
                   .setStatusCode(StatusCodes::INTERNAL_SERVER_ERROR)
                   .build(),
                   client_fd);
    close(client_fd);
  }
}
} // namespace handlers
} // namespace httpxx