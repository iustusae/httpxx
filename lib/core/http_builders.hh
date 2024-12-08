#pragma once

#include "http_enums.hh"
#include "http_objects.hh"
#include <optional>
namespace httpxx::http {

struct ResponseBuilder {
  ResponseBuilder() : built_reponse(Response{}) {}


  auto setContentType(ContentType ct)
  {
      this->setHeader("Content-Type", contentTypeToString(ct));
    return *this;
  }
  auto setStartLine(float http_version, StatusCodes status_code)
      -> ResponseBuilder & {
    built_reponse.start_line = {http_version, status_code};
    return *this;
  }

  auto setStatusCode(StatusCodes status_code) -> ResponseBuilder & {
    return this->setStartLine(1.1, status_code);
  }
  auto setHeader(const std::string &header, const std::string &value)
      -> ResponseBuilder & {
    built_reponse.hd.at(header) = value;
    return *this;
  }

  auto setBody(const std::optional<std::string> &body) -> ResponseBuilder & {
    built_reponse.response_body = body;
    return *this;
  }
  auto build() { return built_reponse; }

private:
  Response built_reponse{};
};

} // namespace httpxx::http
