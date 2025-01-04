
#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

#include "httpxx/configuration.hh"
#include "httpxx/request_handlers.hh"
#include "httpxx/router.hh"
#include "httpxx/socket_enums.hh"

namespace httpxx {

class httpxSocketException : public std::exception {
  const std::string_view _message;

 public:
  explicit httpxSocketException(const std::string_view message)
      : _message(message) {}

  [[nodiscard]] const char* what() const noexcept override {
    return _message.data();
  }
};

inline void handle_socket_error(const std::string& prefix) {
  switch (errno) {
    case EBADF:
      throw httpxSocketException(prefix + " Invalid file descriptor.");
    case EFAULT:
      throw httpxSocketException(prefix + " Invalid address.");
    case EINVAL:
      throw httpxSocketException(prefix + " Invalid argument.");
    case ENOPROTOOPT:
      throw httpxSocketException(prefix +
                                 " Unknown option at the indicated level.");
    case ENOTSOCK:
      throw httpxSocketException(prefix + " Not a socket.");
    default:
      throw std::runtime_error(
          prefix + " Unknown error: " + std::string(strerror(errno)));
  }
}

class Socket {
  using SocketOptionValue = const int;
  using Port = in_port_t;

 private:
  int _fd{};
  AddressFamilies _af{};
  struct sockaddr_in server_addr{};

 public:
  [[nodiscard]] static int init_socket(
      AddressFamilies domain, SocketType type,
      Protocol protocol = Protocol::ip) noexcept(false);
  void bind_socket(in_port_t port, const std::string& ip_address);
  auto SetSocketOption(SocketOptions option, SocketOptionValue option_value)
      -> Socket*;

  [[nodiscard]] auto Accept() const -> int;

  static auto Write(const int client_fd, const std::string& message) -> void;

  auto Listen(const httpxx::Router& router, const httpxx::Config& config,
              const int max_queued_connections = SOMAXCONN) const -> void;

  Socket() = default;

  Socket(const AddressFamilies af, const SocketType type,
         const Protocol protocol)
      : _fd(init_socket(af, type, protocol)), _af(af) {
    std::clog << "_fd: " << _fd << std::endl;
  }
};

inline void Socket::bind_socket(in_port_t port, const std::string& ip_address) {
  server_addr.sin_port = htons(port);

  if (ip_address.empty()) {
  } else {
    if (inet_pton(AF_INET, ip_address.c_str(), &server_addr.sin_addr) <= 0) {
      std::cerr << "Invalid IP address: " << ip_address << std::endl;
      close(_fd);
      throw httpxSocketException("[httpx::Socket::Bind] Invalid IP address.");
    }
  }

  const int bind_res =
      bind(_fd, reinterpret_cast<struct sockaddr*>(&server_addr),
           sizeof(server_addr));
  if (bind_res == -1) {
    handle_socket_error("[httpx::Socket::Bind]");
  }

  std::clog << "Server is listening at port: " << port << std::endl;
}

inline auto Socket::SetSocketOption(SocketOptions option,
                                    SocketOptionValue option_value) -> Socket* {
  const int opt =
      setsockopt(_fd, as_integer(SocketLevel::sol_socket), as_integer(option),
                 &option_value, sizeof(option_value));

  if (opt != 0) {
    handle_socket_error("[httpx::Socket::SetSocketOption]");
  }

  return this;
}

[[nodiscard]] inline auto Socket::Accept() const -> int {
  struct sockaddr_in client_addr{};
  int client_addr_len = sizeof(client_addr);
  const int client_fd =
      accept(_fd, reinterpret_cast<struct sockaddr*>(&client_addr),
             reinterpret_cast<socklen_t*>(&client_addr_len));

  if (client_fd == -1) {
    handle_socket_error("[httpx::Socket::Accept]");
  }

  return client_fd;
}

inline auto Socket::Write(const int client_fd, const std::string& message)
    -> void {
  const auto sz = write(client_fd, message.c_str(), message.size());
  httpxx_assert(sz == static_cast<std::ptrdiff_t>(message.size()),
                "writing went wrong.");
  close(client_fd);
}

inline auto Socket::Listen(const httpxx::Router& router,
                           const httpxx::Config& config,
                           const int max_queued_connections) const -> void {
  if (listen(_fd, max_queued_connections) != 0) {
    throw httpxSocketException(
        "[httpx::Socket::Listen] Failed to initialize listening.");
  }

  while (true) {
    int client_fd = Accept();
    if (client_fd < 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    char buffer[4096];
    bzero(&buffer, 4096);
    if (read(client_fd, &buffer, 4096) == -1) {
      close(client_fd);
      continue;
    }
    std::jthread{&httpxx::RequestHandler::handle, router, config, client_fd,
                 buffer}
        .detach();
  }
}
[[nodiscard]] inline int Socket::init_socket(
    AddressFamilies domain, SocketType type,
    Protocol protocol) noexcept(false) {
  int socket_fd =
      socket(as_integer(domain), as_integer(type), as_integer(protocol));

  if (socket_fd == -1) {
    switch (errno) {
      case EACCES:
        throw httpxSocketException("Permission denied to create socket");
      case EAFNOSUPPORT:
        throw httpxSocketException("Address family not supported");
      case EMFILE:
        throw httpxSocketException("Process file descriptor table is full");
      case ENFILE:
        throw httpxSocketException("System file descriptor table is full");
      case EPROTONOSUPPORT:
        throw httpxSocketException("Protocol not supported");
      case EPROTOTYPE:
        throw httpxSocketException("Protocol wrong type for socket");
      default:
        throw httpxSocketException("Unknown error creating socket: " +
                                   std::string(strerror(errno)));
    }
  }

  return socket_fd;
}
}  // namespace httpxx
