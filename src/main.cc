
#include "../lib/core/http_builders.hh"
#include "../lib/core/router.hh"
#include "../lib/server/Server.hh"
#include "core/http_enums.hh"
#include "core/http_objects.hh"

int main(int argc, char *argv[]) {

  httpxx::Router router;
  router.add_endpoint("/", [&](int client_fd, const httpxx::Request request) {
    std::clog << "/ <-\n";
    auto res = httpxx::http::ResponseBuilder{}
                   .setStatusCode(httpxx::StatusCodes::OK)
                   ->setHeader("Content-Type", "text/html")
                   ->setBody("<p>Hello, world!</p>")
                   ->build();
    std::clog << res.toString() << "lewl";

    return res;
  });

  router.add_endpoint("/echo",
                      [&](int client_fd, const httpxx::Request request) {
                        auto res = httpxx::http::ResponseBuilder{}
                                       .setStatusCode(httpxx::StatusCodes::OK)
                                       ->setHeader("Content-Type", "text/html")
                                       ->setBody(request.body.value())
                                       ->build();
                        std::clog << res.toString() << "lewl";

                        return res;
                      });

  const httpxx::Server server{router, 9999};
  server.start();
}
