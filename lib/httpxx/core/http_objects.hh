#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "http_enums.hh"
#include "http_headers.hh"

namespace httpxx {
constexpr std::string_view HTTP_VERSION{"HTTP/1.1"};

// TODO: Apparently this is a "fold expression" to help with pattern matching,
// so we can assign every variant type to a function. Interesting, gotta learn
// more.
template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;

inline float extractHttpVersion(const std::string& http_version);

using Body = std::string;

struct RequestLine {
  HttpMethod method{};
  std::string uri{};
  float http_version{};
};

inline std::ostream& operator<<(std::ostream& out, const RequestLine& rl) {
  out << "RequestLine {" << rl.method << ", " << rl.uri << ", "
      << rl.http_version << "}";

  return out;
}

struct Request {
  using parameter_t = std::unordered_map<std::string, std::string>;
  RequestLine request_line{};
  Headers headers;
  std::optional<Body> body;
  parameter_t req_parameters{};

  explicit Request(RequestLine request_line, Headers headers,
                   const std::optional<Body>& body,
                   const parameter_t& parameters)
      : request_line(std::move(request_line)),
        headers(std::move(headers)),
        body(body),
        req_parameters(parameters) {}

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
  std::variant<std::monostate, std::string, std::vector<char>> response_body{
      std::monostate{}};

  bool hasBody() const {
    return !std::holds_alternative<std::monostate>(response_body);
  }

  std::string toString() const {
    std::ostringstream oss{};

    // Start line
    oss << std::format("HTTP/{} {} {}\r\n", start_line.http_version,
                       static_cast<int>(start_line.status_code),
                       +start_line.status_code);

    // Headers
    for (const auto& [k, v] : hd) {
      if (v.has_value()) {
        oss << k << ": " << v.value() << "\r\n";
      }
    }
    oss << "\r\n";

    return oss.str();
  }

  // Utility method to get body as string
  std::string getBodyAsString() const {
    return std::visit(overload{[](std::monostate) { return std::string{}; },
                               [](const std::string& str) { return str; },
                               [](const std::vector<char>& vec) {
                                 return std::string(vec.begin(), vec.end());
                               }},
                      response_body);
  }
};
}  // namespace httpxx
