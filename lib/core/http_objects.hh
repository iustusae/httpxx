#pragma once

#include "http_enums.hh"
#include "http_headers.hh"
#include <cassert>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace httpxx {
constexpr std::string_view HTTP_VERSION{"HTTP/0.1"};

// Utility template for visitor pattern
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;
inline float extractHttpVersion(const std::string &http_version);

using Body = std::string;

struct RequestLine {
  HttpMethod method{};
  std::string uri{};
  float http_version{};
};

inline std::ostream &operator<<(std::ostream &out, const RequestLine &rl);

struct Request {
  RequestLine request_line{};
  Headers headers;
  std::optional<Body> body;

  explicit Request(const char *request) {}

  explicit Request(RequestLine request_line, Headers headers,
                   const std::optional<Body> &body)
      : request_line(std::move(request_line)), headers(std::move(headers)),
        body(body) {}

  Request() = default;

  bool requestsFile() const {
    auto uri = request_line.uri;
    uri.erase(uri.find_first_of('/'), 1);
    return uri.find('.') != std::string::npos;
  }
};

struct StartLine {
  float http_version{};
  StatusCodes status_code{};
};

struct Response {
  StartLine start_line{};
  Headers hd{response_headers};
  std::variant<std::monostate, std::string, std::vector<char>> response_body{std::monostate{}};

  bool hasBody() const {
    return !std::holds_alternative<std::monostate>(response_body);
  }

  std::string toString() const {
    std::ostringstream oss{};

    // Start line
    oss << std::format("HTTP/{} {} {}\r\n",
                       start_line.http_version,
                       static_cast<int>(start_line.status_code),
                       +start_line.status_code);

    // Headers
    for (const auto &[k, v] : hd) {
      if (v.has_value()) {
        oss << k << ": " << v.value() << "\r\n";
      }
    }
    oss << "\r\n";

    return oss.str();
  }

  // Utility method to get body as string
  std::string getBodyAsString() const {
    return std::visit(overload{
      [](std::monostate) { return std::string{}; },
      [](const std::string& str) { return str; },
      [](const std::vector<char>& vec) {
        return std::string(vec.begin(), vec.end());
      }
    }, response_body);
  }
};


} // namespace httpxx
