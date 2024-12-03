#pragma once

#include "http_enums.hh"
#include "http_headers.hh"
#include <cassert>
#include <iostream>
#include <optional>
#include <string>
#include <utility>

namespace httpxx {
constexpr std::string_view HTTP_VERSION{"HTTP/0.1"};

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
  std::optional<std::string> response_body{std::nullopt};

  std::string toString() const {
    std::ostringstream oss{};

    oss << std::format("HTTP/{} {} {}", start_line.http_version,
                       static_cast<int>(start_line.status_code),
                       +start_line.status_code);

    if (!areEmpty(hd)) {
      oss << "\n";
      for (const auto &[k, v] : hd) {
        if (!v.has_value()) {
          continue;
        } else {
          oss << k << ':' << v.value() << '\n';
        }
      }

      oss << "\r\n\r\n";
    }

    if (response_body.has_value()) {
      oss << response_body.value();
    }

    return oss.str();
  };
};

} // namespace httpxx
