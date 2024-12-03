#pragma once
#include "../core/http_handlers.hh"
#include "../core/router.hh"
#include "opts.hh"
#include <arpa/inet.h>
#include <atomic>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <queue>
#include <source_location>
#include <stdexcept>
#include <string_view>
#include <strings.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace httpx {

class httpxSocketException : public std::exception {
  const std::string_view _message;

public:
  explicit httpxSocketException(const std::string_view message)
      : _message(message) {}
  [[nodiscard]] const char *what() const noexcept override {
    return _message.data();
  }
};

class Socket {
  using SocketOptionValue = const int;
  using Port = in_port_t;

private:
  [[nodiscard]] static int
  init_socket(AddressFamilies domain, SocketType type,
              Protocol protocol = Protocol::ip) noexcept(false);

private:
  int _fd{};
  AddressFamilies _af{};
  struct sockaddr_in server_addr{};

public:
  auto SetSocketOption(SocketOptions option, SocketOptionValue option_value) {
    int opt =
        setsockopt(_fd, as_integer(SocketLevel::sol_socket), as_integer(option),
                   &option_value, sizeof(option_value));
    if (opt != 0) {
      switch (errno) {
      case EBADF:
        throw httpxSocketException("[httpx::Socket::SetSocketOption] The file "
                                   "descriptor is not valid.");
      case EFAULT:
        throw httpxSocketException("[httpx::Socket::SetSocketOption] The "
                                   "function screwed up and didnt give a "
                                   "valid adress space for opt_val");
      case EINVAL:
        throw httpxSocketException("[httpx::Socket::SetSocketOption] "
                                   "sizeof(option_value) is invalid.");
      case ENOPROTOOPT:
        throw httpxSocketException("[httpx::Socket::SetSocketOption] The "
                                   "option is unknown at the indicated level.");
      case ENOTSOCK:
        throw httpxSocketException(
            "[httpx::Socket::SetSocketOption] Something went wrong, the "
            "current fd does not refer to a socket.");
      default:
        throw std::runtime_error("how");
      }
    }
    return this;
  }
  auto Bind(in_port_t port, const std::string &ip_address = "",
            std::source_location location = std::source_location()) -> void {
    server_addr.sin_port = htons(port);
    if (ip_address.empty()) {
      server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available address
    } else {
      // Convert IP address to network format
      if (inet_pton(AF_INET, ip_address.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << ip_address;

        close(_fd);
      }
    };
    int bind_res = bind(_fd, reinterpret_cast<struct sockaddr *>(&server_addr),
                        sizeof(server_addr));

    if (bind_res == -1) {
      switch (errno) {
      case EACCES:
        throw httpxSocketException("[httpx::Socket::Bind] EACCES");
      case EADDRINUSE:
        throw httpxSocketException("[httpx::Socket::Bind] EADDRINUSE");
      case EBADF:
        throw httpxSocketException("[httpx::Socket::Bind] EBADF");
      case EINVAL:
        throw httpxSocketException("[httpx::Socket::Bind] EINVAL");
      case ENOTSOCK:
        throw httpxSocketException("[httpx::Socket::Bind] ENOTSOCK");
      case EADDRNOTAVAIL:
        throw httpxSocketException("[httpx::Socket::Bind] EADDRNOTAVAIL");
      case EFAULT:
        throw httpxSocketException("[httpx::Socket::Bind] EFAULT");
      case ELOOP:
        throw httpxSocketException("[httpx::Socket::Bind] ELOOP");
      case ENAMETOOLONG:
        throw httpxSocketException("[httpx::Socket::Bind] ENAMETOOLONG");
      case ENOENT:
        throw httpxSocketException("[httpx::Socket::Bind] ENOENT");
      case ENOTDIR:
        throw httpxSocketException("[httpx::Socket::Bind] ENOTDIR");
      case EROFS:
        throw httpxSocketException("[httpx::Socket::Bind] EROFS");
      default:
        throw std::runtime_error("[bind] how?");
      }
    }

    std::clog << "Server is listening at port: " << port << std::endl;
  }

  [[nodiscard]] auto Accept() const -> int {

    struct sockaddr_in client_addr{};
    int client_addr_len = sizeof(client_addr);
    int client_fd =
        accept(_fd, reinterpret_cast<struct sockaddr *>(&client_addr),
               reinterpret_cast<socklen_t *>(&client_addr_len));

    if (client_fd == -1) {
      switch (errno) {
      case EAGAIN:
        // std::clog << "[httpx::Socket::Accept] No connections to accept, "
        //              "non-blocking socket\n";
        return -1;
      case EBADF:
        throw httpxSocketException(
            "[httpx::Socket::Accept] EBADF: Invalid file descriptor");
      case ECONNABORTED:
        throw httpxSocketException(
            "[httpx::Socket::Accept] ECONNABORTED: Connection aborted");
      case EFAULT:
        throw httpxSocketException(
            "[httpx::Socket::Accept] EFAULT: Invalid address argument");
      case EINTR:
        throw httpxSocketException(
            "[httpx::Socket::Accept] EINTR: Interrupted by signal");
      case EINVAL:
        throw httpxSocketException("[httpx::Socket::Accept] EINVAL: Socket not "
                                   "listening or invalid address length");
      case EMFILE:
        throw httpxSocketException(
            "[httpx::Socket::Accept] EMFILE: Too many open file descriptors");
      case ENFILE:
        throw httpxSocketException("[httpx::Socket::Accept] ENFILE: "
                                   "System-wide limit on open files reached");
      case ENOBUFS:
      case ENOMEM:
        throw httpxSocketException("[httpx::Socket::Accept] ENOBUFS/ENOMEM: "
                                   "Insufficient memory or buffer space");
      case ENOTSOCK:
        throw httpxSocketException(
            "[httpx::Socket::Accept] ENOTSOCK: Invalid socket file descriptor");
      case EOPNOTSUPP:
        throw httpxSocketException(
            "[httpx::Socket::Accept] EOPNOTSUPP: Unsupported socket type");
      case EPERM:
        throw httpxSocketException(
            "[httpx::Socket::Accept] EPERM: Firewall or security restrictions");
      case EPROTO:
        throw httpxSocketException(
            "[httpx::Socket::Accept] EPROTO: Protocol error");
      default:
        throw std::runtime_error("[httpx::Socket::Accept] Unexpected error: " +
                                 std::string(strerror(errno)));
      }
    }
    return client_fd;
  }

  static auto Write(int client_fd, const std::string &message) -> void {
    int sz = write(client_fd, message.c_str(), message.size());
    close(client_fd);
  }

  auto Listen(const httpxx::Router &router,
              int max_queued_connections = SOMAXCONN) const -> void {
    std::atomic<int> connection_count{0};

    // Set up the initial listen
    if (listen(_fd, max_queued_connections) != 0) {
      throw httpxSocketException(
          "[httpx::Socket::Listen] Failed to initialize listening");
    }

    while (true) {
      int client_fd = Accept();
      if (client_fd < 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        continue;
      }

      std::clog << "\nclient #" << ++connection_count << "connected\n";
      char buffer[4096];
      bzero(&buffer, 4096);
      if (read(client_fd, &buffer, 4096) == -1) {
        close(client_fd);
        continue;
      }
      std::clog << "[" << connection_count << "]'s request: \n" << buffer;
      std::jthread{&httpxx::handlers::handle_request, std::move(router),
                   std::move(client_fd), std::move(buffer)};
    }
  }

  auto mt_Listen(const httpxx::Router &router,
                 int max_queued_connections = SOMAXCONN) const -> void {
    std::atomic<int> connection_count{0};

    // Set up the initial listen
    if (listen(_fd, max_queued_connections) != 0) {
      throw httpxSocketException(
          "[httpx::Socket::Listen] Failed to initialize listening");
    }

    // Number of available CPU cores
    unsigned int num_threads = std::thread::hardware_concurrency();
    std::clog << "Using " << num_threads << " threads for handling requests\n";

    // Thread-safe queue to hold client connections
    std::queue<std::tuple<int, httpxx::Router>> request_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;

    // Worker function to handle requests
    auto worker = [&]() {
      while (true) {
        std::tuple<int, httpxx::Router> client_request;
        {
          std::unique_lock lock(queue_mutex);
          queue_cv.wait(lock, [&] { return !request_queue.empty(); });
          client_request = std::move(request_queue.front());
          request_queue.pop();
        }

        // Process the client request
        int client_fd = std::get<0>(client_request);
        auto &router = std::get<1>(client_request);
        char buffer[4096];
        bzero(&buffer, 4096);
        if (read(client_fd, &buffer, 4096) == -1) {
          close(client_fd);
          continue;
        }

        std::clog << "[" << ++connection_count << "]'s request: \n" << buffer;
        httpxx::handlers::handle_request(router, client_fd, buffer);
        close(client_fd);
      }
    };

    // Create worker threads
    std::vector<std::jthread> workers;
    for (unsigned int i = 0; i < num_threads; ++i) {
      workers.emplace_back(worker);
    }

    // Accept connections and enqueue requests
    while (true) {
      int client_fd = Accept();
      if (client_fd < 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        continue;
      }

      std::clog << "\nclient #" << ++connection_count << " connected\n";
      {
        std::lock_guard lock(queue_mutex);
        request_queue.push({client_fd, router});
      }
      queue_cv.notify_one(); // Notify one worker that a request is available
    }
  }

public:
  Socket() = default;
  Socket(const AddressFamilies af, const SocketType type,
         const Protocol protocol)
      : _fd(init_socket(af, type, protocol)), _af(af) {
    std::clog << "_fd: " << _fd << std::endl;
  }
};

} // namespace httpx
