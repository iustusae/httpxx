#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>
#include <string>
#include <unordered_map>
namespace httpxx {

enum class HttpMethod {
  GET = 0,
  POST,
  PUT,
  DELETE,
  http_method_count,
};

inline std::ostream &operator<<(std::ostream &out, const HttpMethod method) {

  switch (method) {

  case HttpMethod::GET:
    out << "GET";
    break;
  case HttpMethod::POST:
    out << "POST";
    break;
  case HttpMethod::PUT:
    out << "PUT";
    break;
  case HttpMethod::DELETE:
    out << "DELETE";
    break;
  case HttpMethod::http_method_count:
    break;
  }

  return out;
}

inline std::string operator+(const HttpMethod &method) {
  std::string str{};

  switch (method) {

  case HttpMethod::GET:
    str = "GET";
    break;
  case HttpMethod::POST:
    str = "POST";
    break;
  case HttpMethod::PUT:
    str = "PUT";
    break;
  case HttpMethod::DELETE:
    str = "DELETE";
    break;
  case HttpMethod::http_method_count:
    break;
  }

  return str;
}

inline HttpMethod stringToHttpMethod(const std::string &method) {
  static const std::unordered_map<std::string, HttpMethod> strToHM{
      {"GET", HttpMethod::GET},
      {"POST", HttpMethod::POST},
      {"PUT", HttpMethod::PUT},
      {"DELETE", HttpMethod::DELETE}

  };

  assert(strToHM.find(method) != strToHM.end());

  return strToHM.at(method);
}

enum class StatusCodes {
  /*1xx Informational*/
  CONTINUE = 100,
  PRTCL_SWITCH = 101,

  /*2xx Successful*/
  OK = 200,
  CREATED = 200,
  ACCEPTED = 202,
  NON_AUTH_INFO = 203,
  NO_CONTENT = 204,
  RESET_CONTENT = 205,
  PARTIAL_CONTENT = 206,

  /*3xx Redirection*/
  MULTIPLE_CHOICE = 300,
  MOVED_PERMANETLY = 301,
  FOUND = 302,
  SEE_OTHER = 303,
  NOT_MODIFIED = 304,
  USE_PROXY = 305,
  TEMP_REDIRECT = 307,

  /*4xx Client Error*/
  UNAUTHORIZED = 401,
  PAYMENT_REQUIRED = 402,
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  METHOD_NOT_ALLOWED = 405,
  NOT_ACCEPTABLE = 406,
  PROXY_AUTH_REQUIRED = 407,
  REQUEST_TIMEOUT = 408,
  CONFLICT = 409,
  GONE = 410,
  LENGTH_REQUIRED = 411,
  PRECONDITION_FAILED = 412,
  REQ_ENTITY_TOO_LARGE = 413,
  REQ_URI_TOO_LONG = 414,
  UNSUPPORTED_MEDIA_TYPE = 415,
  REQ_RANGE_NOT_SATISFIABLE = 416,
  EXPECTATION_FAILED = 417,

  /*5xx Server Error*/
  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED = 501,
  BAD_GATEWAY = 502,
  SERVICE_UNAVAILABLE = 503,
  GATEWAY_TIMEOUT = 504,
  HTTP_VERSION_NOT_SUPPORTED = 505

};

inline std::string operator+(const StatusCodes &method) {
  std::string str{};

  switch (method) {
  case StatusCodes::CONTINUE:
    str = "CONTINUE";
    break;
  case StatusCodes::PRTCL_SWITCH:
    str = "PRTCL_SWITCH";
    break;
  case StatusCodes::OK:
    str = "OK";
    break;
  case StatusCodes::ACCEPTED:
    str = "ACCEPTED";
    break;
  case StatusCodes::NON_AUTH_INFO:
    str = "NON_AUTH_INFO";
    break;
  case StatusCodes::NO_CONTENT:
    str = "NO_CONTENT";
    break;
  case StatusCodes::RESET_CONTENT:
    str = "RESET_CONTENT";
    break;
  case StatusCodes::PARTIAL_CONTENT:
    str = "PARTIAL_CONTENT";
    break;
  case StatusCodes::MULTIPLE_CHOICE:
    str = "MULTIPLE_CHOICE";
    break;
  case StatusCodes::MOVED_PERMANETLY:
    str = "MOVED_PERMANETLY";
    break;
  case StatusCodes::FOUND:
    str = "FOUND";
    break;
  case StatusCodes::SEE_OTHER:
    str = "SEE_OTHER";
    break;
  case StatusCodes::NOT_MODIFIED:
    str = "NOT_MODIFIED";
    break;
  case StatusCodes::USE_PROXY:
    str = "USE_PROXY";
    break;
  case StatusCodes::TEMP_REDIRECT:
    str = "TEMP_REDIRECT";
    break;
  case StatusCodes::UNAUTHORIZED:
    str = "UNAUTHORIZED";
    break;
  case StatusCodes::PAYMENT_REQUIRED:
    str = "PAYMENT_REQUIRED";
    break;
  case StatusCodes::FORBIDDEN:
    str = "FORBIDDEN";
    break;
  case StatusCodes::NOT_FOUND:
    str = "NOT_FOUND";
    break;
  case StatusCodes::METHOD_NOT_ALLOWED:
    str = "METHOD_NOT_ALLOWED";
    break;
  case StatusCodes::NOT_ACCEPTABLE:
    str = "NOT_ACCEPTABLE";
    break;
  case StatusCodes::PROXY_AUTH_REQUIRED:
    str = "PROXY_AUTH_REQUIRED";
    break;
  case StatusCodes::REQUEST_TIMEOUT:
    str = "REQUEST_TIMEOUT";
    break;
  case StatusCodes::CONFLICT:
    str = "CONFLICT";
    break;
  case StatusCodes::GONE:
    str = "GONE";
    break;
  case StatusCodes::LENGTH_REQUIRED:
    str = "LENGTH_REQUIRED";
    break;
  case StatusCodes::PRECONDITION_FAILED:
    str = "PRECONDITION_FAILED";
    break;
  case StatusCodes::REQ_ENTITY_TOO_LARGE:
    str = "REQ_ENTITY_TOO_LARGE";
    break;
  case StatusCodes::REQ_URI_TOO_LONG:
    str = "REQ_URI_TOO_LONG";
    break;
  case StatusCodes::UNSUPPORTED_MEDIA_TYPE:
    str = "UNSUPPORTED_MEDIA_TYPE";
    break;
  case StatusCodes::REQ_RANGE_NOT_SATISFIABLE:
    str = "REQ_RANGE_NOT_SATISFIABLE";
    break;
  case StatusCodes::EXPECTATION_FAILED:
    str = "EXPECTATION_FAILED";
    break;
  case StatusCodes::INTERNAL_SERVER_ERROR:
    str = "INTERNAL_SERVER_ERROR";
    break;
  case StatusCodes::NOT_IMPLEMENTED:
    str = "NOT_IMPLEMENTED";
    break;
  case StatusCodes::BAD_GATEWAY:
    str = "BAD_GATEWAY";
    break;
  case StatusCodes::SERVICE_UNAVAILABLE:
    str = "SERVICE_UNAVAILABLE";
    break;
  case StatusCodes::GATEWAY_TIMEOUT:
    str = "GATEWAY_TIMEOUT";
    break;
  case StatusCodes::HTTP_VERSION_NOT_SUPPORTED:
    str = "HTTP_VERSION_NOT_SUPPORTED";
    break;
  }

  return str;
}

} // namespace httpxx
