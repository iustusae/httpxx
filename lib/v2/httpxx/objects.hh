#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "enums.hh"

template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overload(Ts...) -> overload<Ts...>;
namespace httpxx {

using header_t = std::unordered_map<std::string, std::string>;
struct Request {
  using parameter_t = std::unordered_map<std::string, std::string>;

  HttpMethod method{};
  std::string uri{};
  std::optional<std::string> body{};
  header_t headers{};
  parameter_t request_parameters{};

  [[nodiscard]] bool requestsFile() const {
    if (uri.empty() || uri == "/") {
      return false;
    }

    auto lastDot = uri.find_last_of('.');
    auto lastSlash = uri.find_last_of('/');

    if (lastDot != std::string::npos &&
        (lastSlash == std::string::npos || lastDot > lastSlash)) {
      return true;
    }

    return false;
  }
};

struct Response {
  using response_body_t =
      std::variant<std::monostate, std::string, std::vector<char>>;

  StatusCodes status_code{};
  header_t headers{};
  response_body_t body{std::monostate{}};

  [[nodiscard]] std::string toString() const {
    std::stringstream ss{};

    ss << "HTTP/1.1 " << static_cast<int>(status_code) << " " << +status_code
       << "\r\n";

    for (const auto& [key, value] : headers) {
      ss << key << ": " << value << "\r\n";
    }

    ss << "\r\n";

    std::visit(overload{[](const std::monostate&) { /* Empty body */ },
                        [&ss](const std::string& str) { ss << str; },
                        [&ss](const std::vector<char>& vec) {
                          ss.write(vec.data(), vec.size());
                        }},
               body);

    return ss.str();
  }
};

class ResponseBuilder {
 public:
  static ResponseBuilder ok() {
    return ResponseBuilder().status(StatusCodes::OK);
  }

  static ResponseBuilder created() {
    return ResponseBuilder().status(StatusCodes::CREATED);
  }

  static ResponseBuilder badRequest() {
    return ResponseBuilder().status(StatusCodes::BAD_REQUEST);
  }

  static ResponseBuilder notFound() {
    return ResponseBuilder().status(StatusCodes::NOT_FOUND);
  }

  ResponseBuilder& status(const StatusCodes code) {
    response.status_code = code;
    return *this;
  }

  ResponseBuilder& header(const std::string& key, const std::string& value) {
    response.headers[std::move(key)] = std::move(value);
    return *this;
  }

  ResponseBuilder& contentType(const std::string type) {
    return header("Content-Type", std::move(type));
  }

  ResponseBuilder& contentType(const ContentType type) {
    return header("Content-Type", contentTypeToString(type));
  }

  ResponseBuilder& body(const std::string& content) {
    response.body = content;
    setContentLength(content.length());
    return *this;
  }

  ResponseBuilder& body(const std::vector<char>& content) {
    response.body = std::move(content);
    setContentLength(std::get<std::vector<char>>(response.body).size());
    return *this;
  }

  ResponseBuilder& body(const char* content) {
    return body(std::string(content));
  }

  ResponseBuilder& json(const nlohmann::json& content) {
    return contentType("application/json").body(content.dump());
  }

  ResponseBuilder& text(const std::string& content) {
    return contentType("text/plain").body(content);
  }

  ResponseBuilder& html(const std::string& content) {
    return contentType("text/html").body(content);
  }

  Response build() { return std::move(response); }

  ResponseBuilder() = default;

 private:
  Response response;

  void setContentLength(size_t length) {
    header("Content-Length", std::to_string(length));
  }
};
}  // namespace httpxx
