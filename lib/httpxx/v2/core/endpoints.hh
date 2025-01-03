#pragma once

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <ostream>
#include <string>

#include "httpxx/core/http_enums.hh"
#include "httpxx/core/http_objects.hh"

struct Endpoint {
  using handler_t = std::function<httpxx::Response(httpxx::Request)>;
  std::string path{};
  std::vector<httpxx::HttpMethod> accepted_methods{};
  handler_t handler{};
};

inline std::ostream& operator<<(
    std::ostream& out,
    const std::initializer_list<httpxx::HttpMethod>& methods) {
  std::copy(std::begin(methods), std::end(methods),
            std::ostream_iterator<httpxx::HttpMethod>(std::cout, ","));
  return out;
}
