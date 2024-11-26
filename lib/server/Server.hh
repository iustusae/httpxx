#pragma once
#include "../wrapper/net.hh"
#include <netinet/in.h>
class Server {
private:
  int server_fd{};
  struct sockaddr_in server_address{};

  void init(in_port_t port, bool reuse) {
    server_fd = httpx::Socket(AddressFamilies::af_inet, SocketType::stream);

    if (reuse) {
    }
  }

  Server(in_port_t port, bool reuse) { init(port, reuse); }
};

class ServerBuilder {};
