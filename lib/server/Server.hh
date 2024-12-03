#pragma once
#include "../wrapper/net.hh"
#include "core/router.hh"
#include <netinet/in.h>

#include <utility>
namespace httpxx {
class Server {
private:
  httpx::Socket socket{};
  Router router{};

public:
  explicit Server(in_port_t port = 8080) {
    socket = httpx::Socket(AddressFamilies::af_inet, SocketType::stream,
                           Protocol::ip);
    socket.SetSocketOption(SocketOptions::so_reuseaddr, true);
    socket.Bind(port);
  }

  explicit Server(Router router, const in_port_t port = 8080) : Server(port) {
    this->router = std::move(router);
  }
  auto start() const { socket.Listen(router); }
  auto mt_start() const { socket.mt_Listen(router); }
  auto getSocket() -> httpx::Socket & { return socket; }
  auto setSocket(const httpx::Socket &socket) { this->socket = socket; }
};
} // namespace httpxx
