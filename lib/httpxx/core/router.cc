#include "router.hh"

#include "http_builders.hh"
#include "http_enums.hh"
#include "http_objects.hh"

namespace httpxx {
void Router::add_endpoint(const std::string& path,
                          const handler_t&& handler_function) {
  endpoints.emplace(path, handler_function);
}

[[nodiscard]] handler_t Router::get_handler_fn(const std::string& path) const {
  if (endpoints.contains(path)) {
    return endpoints.at(path);
  } else {
    return [](const Request&) {
      return http::ResponseBuilder{}
             .setStatusCode(StatusCodes::NOT_FOUND)
             .build();
    };
  }
}

void Router::init() { endpoints = {}; }
} // namespace httpxx