#include "net.hh"

#include <fcntl.h>
#include <sys/socket.h>

#include <cerrno>
#include <stdexcept>

int httpx::Socket::init_socket(const AddressFamilies domain,
                               const SocketType type, const Protocol protocol) {
  const int fd =
      socket(as_integer(domain), as_integer(type), as_integer(protocol));
  if (fd < 0) {
    switch (errno) {
      case EACCES:
        throw httpxSocketException(
            "Permission to create a socket of the "
            "specified type and/or protocol is denied");
      case EAFNOSUPPORT:
        throw httpxSocketException(
            "The implementation does not support the specified address "
            "family.");
      case EINVAL:
        throw httpxSocketException(
            "Unknown protocol, or protocol family not available.");
      case EMFILE:
        throw httpxSocketException(
            "The per-process limit on the number of open "
            "file descriptors has been reached.");
      case ENFILE:
        throw httpxSocketException(
            "The system-wide limit on the total number of "
            "open files has been reached.");
      case ENOBUFS:
      case ENOMEM:
        throw httpxSocketException(
            "Insufficient  memory  is  available.   The socket cannot be "
            "created "
            "until sufficient re‐sources are freed.");
      case EPROTONOSUPPORT:
        throw httpxSocketException(
            "The protocol type or the specified protocol "
            "is not supported within this domain.");
      default:
        throw std::runtime_error("how?");
    }
  }

  // Set for non blocking IO
  const int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    throw httpxSocketException("[httpx::Socket::init_socket]: flags.");
  }
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  return fd;
}