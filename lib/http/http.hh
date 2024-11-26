#include <cstddef>
#include <string>
namespace httpx::http {
static constexpr size_t MAX_BUFFER = 4096;
static constexpr auto HTTP_400 = "HTTP/1.1 400 Not Found\r\n\r\n";
static constexpr auto CLRF = "\r\n\r\n";

const std::string HTTP_200 = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 13\r\n"
                             "\r\n"
                             "Hello, World!";
} // namespace httpx::http
