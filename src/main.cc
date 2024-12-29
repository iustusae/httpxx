#include <httpxx/core/http_builders.hh>
#include <httpxx/core/http_handlers.hh>
#include <httpxx/core/http_objects.hh>
#include <httpxx/server/Server.hh>
#include <httpxx/wrapper/net.hh>
#include <inja/inja.hpp>

#include "httpxx/core/http_enums.hh"

int main(void) {
  const auto app = Config::fromFile("../config.toml");
  httpxx::Router router;
  router.add_endpoint("/", [&](const httpxx::Request& request) {
    return httpxx::handlers::serve_file(
        std::format("{}/index.html", app._www_path));
  });
  router.add_endpoint("/param_test/", [&](const httpxx::Request& request) {
    // Ensure that the request parameters are not empty
    httpxx_assert(not request.req_parameters.empty(), "bruh wtf");

    // Create an Inja JSON object and populate it with request parameters
    inja::json data = request.req_parameters;

    // Debugging: Print out the parameters received
    std::cout << "req params in url\n";
    for (const auto& [k, v] : request.req_parameters) {
      std::cout << "name: " << k << " value: " << v << '\n';
    }

    // Attempt to render the template
    try {
      std::string rendered = inja::render("<h1>Hello, {{ name }}</h1>", data);
      std::cout << rendered << '\n';
      // Send the rendered HTML response
      return httpxx::http::ResponseBuilder{}
          .setStatusCode(httpxx::StatusCodes::OK)
          .setContentType(httpxx::ContentType::TEXT_HTML)
          .setBody(rendered)
          .build();
    } catch (const inja::RenderError& e) {
      std::cerr << "Render error: " << e.what() << '\n';
      return httpxx::http::ResponseBuilder{}
          .setBody("<h1>Error rendering template</h1>")
          .build();
    }
  });

  router.add_endpoint("/jason", [&](const httpxx::Request& request) {
    const std::string json_data = R"({
        "string_id": "unique12345",
        "content": "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
        "metadata": {
            "author": "John Doe",
            "created_at": "2024-12-01T10:00:00Z",
            "updated_at": "2024-12-01T10:30:00Z",
            "version": 1.0,
            "tags": ["sample", "text", "long"]
        },
        "properties": {
            "length": 350,
            "encoding": "UTF-8",
            "format": "plain_text",
            "is_encrypted": false
        },
        "references": [
            {
                "ref_id": "ref001",
                "description": "Placeholder text commonly used in design mockups",
                "source": "https://en.wikipedia.org/wiki/Lorem_ipsum"
            },
            {
                "ref_id": "ref002",
                "description": "Related content on text generation and its history",
                "source": "https://www.lipsum.com/"
            }
        ],
        "related_data": {
            "related_string_id": "unique67890",
            "summary": "This content serves as placeholder text often used in graphic design or testing scenarios."
        }
    })";
    auto res = httpxx::http::ResponseBuilder{}
                   .setStatusCode(httpxx::StatusCodes::OK)
                   .setContentType(httpxx::ContentType::APPLICATION_JSON)
                   .setBody(json_data)
                   .build();

    return res;
  });

  router.add_endpoint("/echo", {[](const httpxx::Request& req) {
                        return httpxx::http::ResponseBuilder{}
                            .setStatusCode(httpxx::StatusCodes::OK)
                            .setHeader("Content-Type", "text/html")
                            .setBody(req.body.value_or(""))
                            .build();
                      }});

  std::clog << std::format("server at: http://localhost:{}, www_path = {}",
                           app._port, app._www_path)
            << '\n';

  httpxx::Server{router, app}.start();
}
