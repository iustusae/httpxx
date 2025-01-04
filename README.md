# httpxx v2: A Simple HTTP/1.1 webserver x backend framework? in C++20

`httpxx` is a lightweight HTTP/1.1 server implementation written in modern C++20. It enables serving static files, handling custom API endpoints, rendering templates, and returning JSON responses. The project is designed to be simple, extensible, and easy to integrate into your own C++ applications.

## Dependencies

- `nlohmann_json` for JSON handling.
- `fmt` for formatting strings.
- `inja` for template rendering (bundled already in `include/inja.hpp`).
- `tomlpp` for template rendering (bundled already in `include/tomlpp.hpp`).
- C++20-compatible compiler.


## Features

- Serve static files (HTML, CSS, JS, images and videos).
- Handle custom API endpoints with support for multiple HTTP methods (GET, POST).
- Render dynamic content using the `inja` C++ template engine.
- Return structured data as JSON using `nlohmann::json`.
- Fully configurable server via a `config.toml` file.

## Example Usage

This section demonstrates how to set up and use the `httpxx` server with a few example endpoints.

### Code Example

```cpp
#include <httpxx/configuration.hh>
#include <httpxx/request_handlers.hh>
#include <httpxx/router.hh>
#include <include/inja.hpp>
#include <nlohmann/json.hpp>
#include "httpxx/server.hh"

int main() {
  try {
    // Load configuration from file
    auto config = httpxx::Config::fromFile("./config.toml");

    // Create router using builder pattern
    auto router = httpxx::RouterBuilder()

        // Example endpoint handling GET and POST requests
        .methods("/endpoint", {httpxx::HttpMethod::GET, httpxx::HttpMethod::POST},
                 [](const httpxx::Request& req) {
                   if (req.method == httpxx::HttpMethod::GET) {
                     return httpxx::ResponseBuilder::ok()
                         .contentType("text/plain")
                         .body("Oh, a GET request!")
                         .build();
                   }
                   // Handle POST
                   return httpxx::ResponseBuilder::ok()
                       .html(fmt::format("<h1>Oh, a POST request!</h1> {}", 
                                         req.body.value_or("request has no body")))
                       .build();
                 })

        // Serve static file (index.html)
        .get("/", [config](const httpxx::Request&) {
          return httpxx::FileServer::serveFile(
              std::filesystem::path(config.getWwwPath()) / "index.html");
        })

        // Example JSON response
        .get("/jason", [](const httpxx::Request&) {
          nlohmann::json data = {
              {"string_id", "unique12345"},
              {"content", "Lorem ipsum dolor sit amet..."},
              {"metadata", {{"author", "John Doe"}, {"created_at", "2024-12-01T10:00:00Z"}}},
              {"properties", {{"length", 350}, {"encoding", "UTF-8"}, {"is_encrypted", false}}}
          };
          return httpxx::ResponseBuilder::ok().json(data).build();
        })

        // Echo request body
        .get("/echo", [](const httpxx::Request& req) {
          return httpxx::ResponseBuilder::ok()
              .html(req.body.value_or(""))
              .build();
        })

        // Parameter test with template rendering
        .get("/param_test", [](const httpxx::Request& request) {
          if (request.request_parameters.empty()) {
            return httpxx::ResponseBuilder::badRequest()
                .html("<h1>Error: Please provide name and email parameters</h1>")
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

            nlohmann::json data;
            for (const auto& [key, value] : request.request_parameters) {
              data[key] = value;
            }

            nlohmann::json parameters;
            for (const auto& [key, value] : request.request_parameters) {
              parameters.push_back({{"first", key}, {"second", value}});
            }
            data["request_parameters"] = parameters;

            auto rendered = env.render(tmpl, data);
            return httpxx::ResponseBuilder::ok().html(rendered).build();
          } catch (const std::exception& e) {
            return httpxx::ResponseBuilder{}
                .html(fmt::format("<h1>Template Error</h1><p>{}</p>", e.what()))
                .build();
          }
        })
        .build();

    // Log server info
    std::clog << fmt::format("Server running at: http://localhost:{}, serving from {}",
                             config.getPort(), config.getWwwPath().string()) << '\n';

    // Start the server
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
```

### Description of Example

1. **Configuration File**:  
   The server reads configuration settings from a `config.toml` file. It includes the server’s port and the path to serve static files.

2. **Endpoints**:
   - **`/`**: Serves the `index.html` file from the configured directory.
   - **`/endpoint`**: Handles both GET and POST requests. For GET, it returns a simple message. For POST, it renders HTML content based on the request body.
   - **`/param_test`**: Demonstrates URL parameter handling and template rendering with `inja`. It renders dynamic content based on the parameters received in the URL.
   - **`/jason`**: Returns a predefined JSON object with various fields.
   - **`/echo`**: Echoes the body of the incoming request.

3. **Server**:
   The server is created by passing the `Router` and configuration to the `httpxx::Server`. The server starts listening for incoming requests once initialized.

### Template Rendering with Inja

The `/param_test` endpoint shows how to use the `inja` template engine to render content dynamically based on request parameters.

### Serving Static Files

The server can serve static files such as HTML, CSS, and JavaScript using `httpxx::FileServer::serveFile()`.



## Configuration File Example (`config.toml`)

Here’s an example of the configuration file:

```toml
[app]
port = 8080
www_path = "/path/to/static/files"
```

## Build Instructions

1. **Clone the repository**:

   ```bash
   git clone https://github.com/your-username/httpxx.git
   cd httpxx
   ```

2. **Create a build directory**:

   ```bash
   mkdir -p build
   cd build
   ```

3. **Configure the project with CMake**:

   ```bash
   cmake ..
   ```

4. **Build the project**:

   ```bash
   make
   ```

5. **Run the server**:

   ```bash
   ./server
   ```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

We welcome contributions to `httpxx`! Feel free to fork the repository, create your feature branches, and submit pull requests. Please ensure that your changes are well-documented and tested.
