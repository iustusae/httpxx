#include "http_handlers.hh"

#include <fmt/format.h>
#include <unistd.h>

#include <cstring>
#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include <sstream>
#include <variant>

#include "http_builders.hh"
#include "http_config.hh"
#include "http_headers.hh"
#include "http_objects.hh"
#include "httpxx/core/http_enums.hh"

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

  std::unordered_map<std::string, std::string> req_parms;

  if (uri.find("?") != std::string::npos) {
    std::clog << "req params in url\n";
    auto split_uri = split(uri, "?");
    rl.uri = split_uri.at(0);
    auto split_params = split(split_uri.at(1), "&");
    std::clog << uri << "\n";
    for (const auto& i : split_params) {
      std::clog << i << " ";
    }
    std::clog << '\n';

    for (const auto& param : split_params) {
      auto pvp = split(param, "=");
      std::clog << "pvp: " << fmt::format("{}, {}", pvp.at(0), pvp.at(1))
                << '\n';
      req_parms.insert({pvp.at(0), pvp.at(1)});
    }
  } else {
    rl.uri = uri;
  }

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

  auto rq = Request{rl, hd, std::make_optional(body), req_parms};
  return rq;
}

void response_write(const Response& response, const int client_fd) {
  // Write headers
  const std::string headers = response.toString();
  write(client_fd, headers.c_str(), headers.length());

  // Write body
  std::visit(overload{[](std::monostate) {},
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

inline std::ostream& operator<<(
    std::ostream& out, const std::vector<httpxx::HttpMethod>& methods) {
  std::copy(std::begin(methods), std::end(methods),
            std::ostream_iterator<httpxx::HttpMethod>(std::cout, ","));
  return out;
}
void handle_request(const httpxx::Router& router, const Config& config,
                    int client_fd, const char* buffer) {
  try {
    const auto req = createRequest(buffer);
    Response res{};
    if (req.requestsFile()) {
      res = serve_file(
          fmt::format("{}{}", config._www_path, req.request_line.uri));

    } else {
      auto ep = router.get_endpoint(req.request_line.uri);
      auto method = req.request_line.method;
      if (std::ranges::find(ep.accepted_methods, method) ==
          std::end(ep.accepted_methods)) {
        std::clog << "method " << +req.request_line.method << " not in "
                  << ep.accepted_methods << "\n";
        res =
            http::ResponseBuilder{}
                .set_json_body(
                    nlohmann::json::parse(fmt::format(
                        "{{\"error\": \"Method {} is not allowed on URI {}\"}}",
                        +req.request_line.method, req.request_line.uri)),
                    StatusCodes::METHOD_NOT_ALLOWED)
                .build();
      } else {
        res = ep.handler(req);
      }
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
}  // namespace handlers
}  // namespace httpxx
