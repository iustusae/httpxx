#include "router.hh"
namespace httpxx {

void Router::add_endpoint(const std::string &path,
                          const handler_t &&handler_function) {
  endpoints.emplace(path, handler_function);
}

[[nodiscard]] handler_t Router::get_handler_fn(const std::string &path) const {

  return endpoints.at(path);
}

void Router::init() { endpoints = {}; }

} // namespace httpxx
