#include "wrapper/net.hh"
#include "wrapper/opts.hh"
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  httpx::Socket socket(AddressFamilies::af_inet, SocketType::stream,
                       Protocol::ip);
  socket.SetSocketOption(SocketOptions::so_reuseaddr, true);
  socket.Bind(9999);
  socket.Listen();
}
