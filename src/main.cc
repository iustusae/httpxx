#include <httpxx/core/http_builders.hh>
#include <httpxx/core/http_handlers.hh>
#include <httpxx/core/http_objects.hh>
#include <httpxx/server/Server.hh>
#include <httpxx/wrapper/net.hh>
#include <include/inja.hpp>
#include <nlohmann/json_fwd.hpp>

#include "httpxx/core/http_enums.hh"

int main(void) {
  const auto app = Config::fromFile("./config.toml");
  httpxx::Router router;
  inja::Environment env{};

  router.add_endpoint(
      "/endpoint", {httpxx::HttpMethod::GET, httpxx::HttpMethod::POST},
      [](const httpxx::Request& req) {
        switch (req.request_line.method) {
          case httpxx::HttpMethod::GET:
            return httpxx::http::ResponseBuilder{}
                .setStatusCode(httpxx::StatusCodes::OK)
                .setContentType(httpxx::ContentType::TEXT_PLAIN)
                .setBody("Oh, a GET request!")
                .build();
          case httpxx::HttpMethod::POST:
            return httpxx::http::ResponseBuilder{}
                .setStatusCode(httpxx::StatusCodes::OK)
                .setContentType(httpxx::ContentType::TEXT_PLAIN)
                .setBody(std::format("<h1>Oh, a POST request!<h1>. {}",
                                     req.body.value_or("request has not body")))
                .build();
          default:

            return httpxx::http::ResponseBuilder{}
                .set_json_body(
                    nlohmann::json::parse(fmt::format(
                        "{{\"error\": \"Method {} is not allowed on URI {}\"}}",
                        +req.request_line.method, req.request_line.uri)),
                    httpxx::StatusCodes::METHOD_NOT_ALLOWED)
                .build();
        }
      });

  router.add_endpoint("/", {httpxx::HttpMethod::GET},
                      [&](const httpxx::Request& request) {
                        return httpxx::handlers::serve_file(
                            std::format("{}/index.html", app._www_path));
                      });
  router.add_endpoint(
      "/param_test/", {httpxx::HttpMethod::GET},
      [&](const httpxx::Request& request) {
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
          std::string rendered = inja::render(
              "<h1>Hello!<h1> <br> <p>Your full name is {{ name }} {{ surname "
              "}}</p> ",
              data);
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
              .setStatusCode(httpxx::StatusCodes::OK)
              .setBody("<h1>Error rendering template</h1>")
              .build();
        }
      });

  router.add_endpoint(
      "/jason", {httpxx::HttpMethod::GET}, [&](const httpxx::Request& request) {
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
                       .set_json_body(nlohmann::json::parse(json_data))
                       .build();

        return res;
      });

  router.add_endpoint("/echo", {httpxx::HttpMethod::GET},
                      {[](const httpxx::Request& req) {
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
