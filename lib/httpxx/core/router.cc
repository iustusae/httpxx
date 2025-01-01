#include "router.hh"

#include "http_builders.hh"
#include "http_enums.hh"
#include "http_objects.hh"
#include "httpxx/v2/core/endpoints.hh"

namespace httpxx {
void Router::add_endpoint(
    const std::string& path,
    const std::initializer_list<HttpMethod>& accepted_methods,
    const handler_t&& handler_function) {
  endpoints.emplace_back(path, accepted_methods, handler_function);
}

[[nodiscard]] handler_t Router::get_handler_fn(const std::string& path) const {
  auto it{std::find_if(std::begin(endpoints), std::end(endpoints),
                       [&](const Endpoint& ep) { return ep.path == path; })};
  if (it != std::end(endpoints)) {
    return it->handler;
  } else {
    return [](const Request&) {
      return http::ResponseBuilder{}
          .setStatusCode(StatusCodes::NOT_FOUND)
          .build();
    };
  }
}

void Router::init() { endpoints = {}; }
}  // namespace httpxx
