#pragma once
#include <string>

namespace httpx {
class Request {
  std::string _method{};
  std::string _target_path{};
  std::string _http_version{};
  std::string _content{};
};
class Response {
  std::string _http_version{};
  std::string status_code{};
};
} // namespace httpx
