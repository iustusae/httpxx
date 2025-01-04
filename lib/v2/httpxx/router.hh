#pragma once

#include <functional>
#include <optional>
#include <string>

#include "httpxx/endpoint.hh"
#include "httpxx/enums.hh"

namespace httpxx {

using handler_t = std::function<Response(Request)>;

class Router {
 public:
  Router() = default;

  void add_endpoint(std::string path, std::vector<HttpMethod> accepted_methods,
                    handler_t handler_function) {
    endpoints.emplace_back(std::move(path), std::move(accepted_methods),
                           std::move(handler_function));
  }

  [[nodiscard]] const Endpoint& get_endpoint(std::string_view path) const {
    static const Endpoint not_found_endpoint{
        "", {}, [](const Request&) {
          return ResponseBuilder{}
              .status(StatusCodes::NOT_FOUND)
              .contentType("application/json")
              .json({{"error", "Endpoint not found"}})
              .build();
        }};

    auto it =
        std::find_if(endpoints.begin(), endpoints.end(),
                     [path](const Endpoint& ep) { return ep.path == path; });

    return it != endpoints.end() ? *it : not_found_endpoint;
  }

  void clear() { endpoints.clear(); }

  [[nodiscard]] size_t size() const { return endpoints.size(); }

  [[nodiscard]] bool has_endpoint(std::string_view path) const {
    return std::any_of(endpoints.begin(), endpoints.end(),
                       [path](const Endpoint& ep) { return ep.path == path; });
  }

  [[nodiscard]] std::vector<std::string> get_registered_paths() const {
    std::vector<std::string> paths;
    paths.reserve(endpoints.size());

    std::transform(endpoints.begin(), endpoints.end(),
                   std::back_inserter(paths),
                   [](const Endpoint& ep) { return ep.path; });

    return paths;
  }

  [[nodiscard]] std::optional<std::vector<HttpMethod>> get_allowed_methods(
      std::string_view path) const {
    auto it =
        std::find_if(endpoints.begin(), endpoints.end(),
                     [path](const Endpoint& ep) { return ep.path == path; });

    if (it != endpoints.end()) {
      return it->accepted_methods;
    }
    return std::nullopt;
  }

  [[nodiscard]] auto begin() const { return endpoints.begin(); }
  [[nodiscard]] auto end() const { return endpoints.end(); }

 private:
  std::vector<Endpoint> endpoints;
};

class RouterBuilder {
 public:
  RouterBuilder& add(std::string path, std::vector<HttpMethod> methods,
                     handler_t handler) {
    router.add_endpoint(std::move(path), std::move(methods),
                        std::move(handler));
    return *this;
  }

  RouterBuilder& get(std::string path, handler_t handler) {
    return add(std::move(path), {HttpMethod::GET}, std::move(handler));
  }

  RouterBuilder& post(std::string path, handler_t handler) {
    return add(std::move(path), {HttpMethod::POST}, std::move(handler));
  }

  RouterBuilder& put(std::string path, handler_t handler) {
    return add(std::move(path), {HttpMethod::PUT}, std::move(handler));
  }

  RouterBuilder& del(std::string path, handler_t handler) {
    return add(std::move(path), {HttpMethod::DELETE}, std::move(handler));
  }

  RouterBuilder& methods(std::string path, std::vector<HttpMethod> methods,
                         handler_t handler) {
    return add(std::move(path), std::move(methods), std::move(handler));
  }

  Router build() { return std::move(router); }

 private:
  Router router;
};
}
