#pragma once
#include "../core/http_config.hh"
#include "../wrapper/net.hh"
#include "core/router.hh"
#include <netinet/in.h>

#include <utility>
namespace httpxx {
class Server {
private:
  httpx::Socket socket{};
  Router router{};
  Config m_config;

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

  explicit Server(Router router, Config  config) : router(std::move(router)), m_config(std::move(config))
  {

    socket = httpx::Socket(AddressFamilies::af_inet, SocketType::stream,
                           Protocol::ip);
    socket.SetSocketOption(SocketOptions::so_reuseaddr, true);
    socket.Bind(m_config._port);
  }

  explicit Server(Config config, const Router& router, const in_port_t port = 8080)
      : Server(router, port) {
    this->m_config = std::move(config);
  }

  auto start() const { socket.Listen(router, m_config); }
  auto getSocket() -> httpx::Socket & { return socket; }
  auto setSocket(const httpx::Socket &socket) { this->socket = socket; }
};
} // namespace httpxx
