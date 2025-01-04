#pragma once

#include "router.hh"
#include "http_objects.hh"
#include <string>
#include <vector>
#include "http_config.hh"

namespace httpxx {
inline std::vector<std::string> split(const std::string& str,
                                      const std::string& delimiter);

namespace handlers {
Request createRequest(const char* _request_string);

void handle_request(const httpxx::Router& router, const Config& config,
                    int client_fd,
                    const char* buffer);
auto serve_file(const std::filesystem::path& path) -> Response;
} // namespace handlers
} // namespace httpxx