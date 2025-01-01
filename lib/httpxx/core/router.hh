#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <variant>

#include "http_objects.hh"
#include "httpxx/v2/core/endpoints.hh"

using handler_t = std::function<httpxx::Response(httpxx::Request)>;

namespace httpxx {
struct Router {
 private:
  std::vector<Endpoint> endpoints;

 public:
  void add_endpoint(const std::string& path,
                    const std::initializer_list<HttpMethod>& accepted_methods,
                    const handler_t&& handler_function);

  [[nodiscard]] handler_t get_handler_fn(const std::string& path) const;

  void init();
};
}  // namespace httpxx
