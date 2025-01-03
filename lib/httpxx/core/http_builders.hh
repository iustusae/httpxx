#pragma once

#include <nlohmann/json.hpp>

#include "http_enums.hh"
#include "http_objects.hh"

namespace httpxx::http {
struct ResponseBuilder {
  ResponseBuilder() : built_response(Response{}) {}

  auto setContentType(ContentType ct) -> ResponseBuilder& {
    this->setHeader("Content-Type", contentTypeToString(ct));
    return *this;
  }

  auto setContentLength(std::size_t len) -> ResponseBuilder& {
    this->setHeader("Content-Length", std::to_string(len));
    return *this;
  }

  auto setStartLine(float http_version, StatusCodes status_code)
      -> ResponseBuilder& {
    built_response.start_line = {http_version, status_code};
    return *this;
  }

  auto setStatusCode(StatusCodes status_code) -> ResponseBuilder& {
    return this->setStartLine(1.1, status_code);
  }

  auto setHeader(const std::string& header, const std::string& value)
      -> ResponseBuilder& {
    built_response.hd[header] = value;
    return *this;
  }

  // Text body setter
  auto setBody(const std::string& body) -> ResponseBuilder& {
    built_response.response_body = body;
    setContentLength(body.size());
    return *this;
  }

  // Binary body setter
  auto setBinaryBody(const std::vector<char>& body) -> ResponseBuilder& {
    built_response.response_body = body;
    setContentLength(body.size());
    return *this;
  }

  auto set_json_body(const nlohmann::json& json_body,
                     const StatusCodes code = StatusCodes::OK) {
    setContentType(ContentType::APPLICATION_JSON);
    setStatusCode(code);
    setBody(json_body.dump());
    return *this;
  }

  auto build() -> Response { return built_response; }

 private:
  Response built_response{};
};
}  // namespace httpxx::http
