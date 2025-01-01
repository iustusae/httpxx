#pragma once

#include <functional>
#include <initializer_list>
#include <string>

#include "httpxx/core/http_enums.hh"
#include "httpxx/core/http_objects.hh"

struct Endpoint {
  using handler_t = std::function<httpxx::Response(httpxx::Request)>;
  std::string path{};
  std::initializer_list<httpxx::HttpMethod> accepted_methods{};
  handler_t handler{};
};
