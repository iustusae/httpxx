#pragma once
#include "http_objects.hh"
#include <functional>
#include <string>
#include <unordered_map>

using handler_t = std::function<httpxx::Response(int, httpxx::Request)>;

namespace httpxx {
struct Router {
private:
  std::unordered_map<std::string, handler_t> endpoints;

public:
  void add_endpoint(const std::string &path,
                    const handler_t &&handler_function);

  [[nodiscard]] handler_t get_handler_fn(const std::string &path) const;

  void init();
};
} // namespace httpxx
