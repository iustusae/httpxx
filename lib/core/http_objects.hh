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
    return std::format("HTTP/{} {} {}\r\n\r\n{} \r\n\r\n{} ",
                       start_line.http_version,
                       static_cast<int>(start_line.status_code),
                       +start_line.status_code, headersToString(hd),
                       (response_body.has_value() ? *response_body : "<lewl>"));
  }
};

} // namespace httpxx
