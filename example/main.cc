#include <httpxx/configuration.hh>
#include <httpxx/request_handlers.hh>
#include <httpxx/router.hh>
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

#include "httpxx/server.hh"

int main() {
  try {
    // Load configuration from file
    auto config = httpxx::Config::fromFile("./config.toml");

    // Create router using builder pattern
    auto router =
        httpxx::RouterBuilder()
            // Basic endpoint demonstrating GET and POST handling
            .methods("/endpoint",
                     {httpxx::HttpMethod::GET, httpxx::HttpMethod::POST},
                     [](const httpxx::Request& req) {
                       if (req.method == httpxx::HttpMethod::GET) {
                         return httpxx::ResponseBuilder::ok()
                             .contentType("text/plain")
                             .body("Oh, a GET request!")
                             .build();
                       }

                       // Handle POST
                       return httpxx::ResponseBuilder::ok()
                           .html(fmt::format(
                               "<h1>Oh, a POST request!</h1> {}",
                               req.body.value_or("request has no body")))
                           .build();
                     })

            // Serve index.html from configured www path
            .get("/",
                 [config](const httpxx::Request&) {
                   return httpxx::FileServer::serveFile(
                       std::filesystem::path(config.getWwwPath()) /
                       "index.html");
                 })

            // Parameter test endpoint with template rendering

            // JSON response example
            .get("/jason",
                 [](const httpxx::Request&) {
                   nlohmann::json data = {
                       {"string_id", "unique12345"},
                       {"content", "Lorem ipsum dolor sit amet..."},
                       {"metadata",
                        {{"author", "John Doe"},
                         {"created_at", "2024-12-01T10:00:00Z"},
                         {"version", 1.0},
                         {"tags", {"sample", "text", "long"}}}},
                       {"properties",
                        {{"length", 350},
                         {"encoding", "UTF-8"},
                         {"is_encrypted", false}}}};

                   return httpxx::ResponseBuilder::ok().json(data).build();
                 })

            // Echo endpoint
            .get("/echo",
                 [](const httpxx::Request& req) {
                   return httpxx::ResponseBuilder::ok()
                       .html(req.body.value_or(""))
                       .build();
                 })
            .get(
                "/param_test",
                [](const httpxx::Request& request) {
                  if (request.request_parameters.empty()) {
                    return httpxx::ResponseBuilder::badRequest()
                        .html(
                            "<h1>Error: Please provide name and email "
                            "parameters</h1>")
                        .build();
                  }

                  try {
                    inja::Environment env;
                    std::string tmpl = R"(
        <html>
        <body>
            <h1>User Information</h1>
            {% if name %}
            <p>Name: {{ name }}</p>
            {% endif %}
            {% if email %}
            <p>Email: {{ email }}</p>
            {% endif %}
            <h2>All Parameters:</h2>
            <ul>
            {% for param in request_parameters %}
                <li>{{ param.first }}: {{ param.second }}</li>
            {% endfor %}
            </ul>
        </body>
        </html>
        )";

                    // Convert request parameters to nlohmann::json
                    nlohmann::json data;
                    for (const auto& [key, value] :
                         request.request_parameters) {
                      data[key] = value;
                    }

                    // Add the full parameters map (as a JSON object) for
                    // iteration in the template
                    nlohmann::json parameters;
                    for (const auto& [key, value] :
                         request.request_parameters) {
                      parameters.push_back({{"first", key}, {"second", value}});
                    }
                    data["request_parameters"] = parameters;

                    // Render the template
                    auto rendered = env.render(tmpl, data);

                    return httpxx::ResponseBuilder::ok().html(rendered).build();
                  } catch (const std::exception& e) {
                    return httpxx::ResponseBuilder{}
                        .html(fmt::format("<h1>Template Error</h1><p>{}</p>",
                                          e.what()))
                        .build();
                  }
                })
            .build();

    // Log server info
    std::clog << fmt::format(
                     "Server running at: http://localhost:{}, www_path = {}",
                     config.getPort(), config.getWwwPath().string())
              << '\n';

    // Create and start server
    auto server = httpxx::Server(std::move(router), std::move(config));
    server.start();

  } catch (const httpxx::ConfigError& e) {
    std::cerr << "Configuration error: " << e.what() << '\n';
    return 1;
  } catch (const std::exception& e) {
    std::cerr << "Server error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
