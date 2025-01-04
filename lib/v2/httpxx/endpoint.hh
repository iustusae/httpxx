#pragma once

#include <functional>

#include "httpxx/objects.hh"

namespace httpxx {
struct Endpoint {
  using handler_t = std::function<httpxx::Response(httpxx::Request)>;
  std::string path{};
  std::vector<httpxx::HttpMethod> accepted_methods{};
  handler_t handler{};
};

}  // namespace httpxx
