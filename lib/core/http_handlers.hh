#pragma once

// clang-format off
#include "router.hh"
#include "http_objects.hh"
// clang-format on
#include <cctype>
#include <string>
#include <vector>

#include "http_config.hh"

namespace httpxx::http
{
    static constexpr size_t MAX_BUFFER = 4096;
    static constexpr auto HTTP_400 = "HTTP/1.1 400 Not Found\r\n\r\n";
    static constexpr auto CLRF = "\r\n\r\n";

    const std::string HTTP_200 = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";
} // namespace httpxx::http
namespace httpxx
{
    inline std::vector<std::string> split(const std::string& str,
                                          const std::string& delimiter);

    namespace handlers
    {
        Request createRequest(const char* _request_string);

        void handle_request(const httpxx::Router& router, const Config& config, int client_fd,
                            const char* buffer);
        auto serve_file(const std::filesystem::path& path) -> Response;
    } // namespace handlers
} // namespace httpxx
