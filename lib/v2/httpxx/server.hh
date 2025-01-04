
#pragma once
#include "httpxx/configuration.hh"
#include "httpxx/router.hh"
#include "httpxx/socket.hh"
#include "httpxx/socket_enums.hh"

namespace httpxx {
class Server {
 private:
  httpxx::Socket socket;
  Router router;
  Config m_config;

 public:
  explicit Server(const in_port_t port = 8080)
      : socket(httpxx::Socket(AddressFamilies::af_inet, SocketType::stream,
                              Protocol::ip)) {
    socket.SetSocketOption(SocketOptions::so_reuseaddr, true);
    socket.bind_socket(port, "");
  }

  explicit Server(Router router, const in_port_t port = 8080) : Server(port) {
    this->router = std::move(router);
  }

  explicit Server(Router router, Config config, const std::string& ip_addr = "")
      : socket(httpxx::Socket(AddressFamilies::af_inet, SocketType::stream,
                              Protocol::ip)),
        router(std::move(router)),
        m_config(std::move(config)) {
    socket.SetSocketOption(SocketOptions::so_reuseaddr, true);
    socket.bind_socket(m_config.getPort(), ip_addr);
  }

  explicit Server(Config config, const Router& router,
                  const in_port_t port = 8080)
      : Server(router, port) {
    this->m_config = std::move(config);
  }

  void start() const { socket.Listen(router, m_config); }

  httpxx::Socket& getSocket() { return socket; }

  void setSocket(const httpxx::Socket& socket) { this->socket = socket; }
};
}  // namespace httpxx
