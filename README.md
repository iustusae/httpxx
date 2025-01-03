# httpxx: A Simple HTTP/1.1 Server in C++20

`httpxx` is a simple HTTP/1.1 server implementation written in modern C++20. The server can serve static files, handle API endpoints, render templates, and respond with JSON data. It provides a basic framework for building custom HTTP servers in C++ with minimal dependencies.

This project is functional but still evolving and is designed to be easy to extend with additional functionality.

## Features

- Serve static files (HTML, CSS, JS, images).
- Handle custom API endpoints with various HTTP methods (GET, POST).
- Support rendering templates using `inja` (a modern C++ template engine).
- Return JSON responses using `nlohmann::json`.
- Configurable server via a `config.toml` file.

## Example Usage

Below is a simple example of how to set up and use the `httpxx` server.

### Code Example

```cpp
#include <httpxx/core/http_builders.hh>
#include <httpxx/core/http_handlers.hh>
#include <httpxx/core/http_objects.hh>
#include <httpxx/server/Server.hh>
#include <httpxx/wrapper/net.hh>
#include <inja/inja.hpp>
#include <nlohmann/json_fwd.hpp>

#include "httpxx/core/http_enums.hh"

int main(void) {
  const auto app = Config::fromFile("./config.toml");
  httpxx::Router router;
  inja::Environment env{};

  // Example of GET and POST request handling
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
                                     req.body.value_or("request has no body")))
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

  // Serve static HTML file
  router.add_endpoint("/", {httpxx::HttpMethod::GET},
                      [&](const httpxx::Request& request) {
                        return httpxx::handlers::serve_file(
                            std::format("{}/index.html", app._www_path));
                      });

  // Example of URL parameters and template rendering with Inja
  router.add_endpoint(
      "/param_test/", {httpxx::HttpMethod::GET},
      [&](const httpxx::Request& request) {
        httpxx_assert(not request.req_parameters.empty(), "Missing parameters!");

        inja::json data = request.req_parameters;

        try {
          std::string rendered = inja::render(
              "<h1>Hello!<h1> <br> <p>Your full name is {{ name }} {{ surname }}</p>",
              data);
          return httpxx::http::ResponseBuilder{}
              .setStatusCode(httpxx::StatusCodes::OK)
              .setContentType(httpxx::ContentType::TEXT_HTML)
              .setBody(rendered)
              .build();
        } catch (const inja::RenderError& e) {
          return httpxx::http::ResponseBuilder{}
              .setStatusCode(httpxx::StatusCodes::OK)
              .setBody("<h1>Error rendering template</h1>")
              .build();
        }
      });

  // Serve JSON data
  router.add_endpoint(
      "/jason", {httpxx::HttpMethod::GET}, [&](const httpxx::Request& request) {
        const std::string json_data = R"({
        "string_id": "unique12345",
        "content": "Lorem ipsum dolor sit amet, consectetur adipiscing elit...",
        "metadata": {
            "author": "John Doe",
            "created_at": "2024-12-01T10:00:00Z"
        },
        "properties": {
            "length": 350,
            "encoding": "UTF-8"
        }
    })";
        return httpxx::http::ResponseBuilder{}
            .set_json_body(nlohmann::json::parse(json_data))
            .build();
      });

  // Echo request body
  router.add_endpoint("/echo", {httpxx::HttpMethod::GET},
                      {[](const httpxx::Request& req) {
                        return httpxx::http::ResponseBuilder{}
                            .setStatusCode(httpxx::StatusCodes::OK)
                            .setHeader("Content-Type", "text/html")
                            .setBody(req.body.value_or(""))
                            .build();
                      }});

  std::clog << std::format("Server started at: http://localhost:{}, serving from {}",
                           app._port, app._www_path)
            << '\n';

  // Start the server
  httpxx::Server{router, app}.start();
}
```

### Description of Example

1. **Configuration File**:
    - The server reads configuration settings from a TOML file (e.g., `config.toml`), which allows you to specify the port number and the directory path for static content.

2. **Endpoints**:
    - The root endpoint (`/`) serves an `index.html` file from the `www_path`.
    - The `/endpoint` endpoint handles both GET and POST requests. For GET requests, it returns a simple string, while POST requests return HTML content.
    - The `/param_test/` endpoint demonstrates handling URL parameters and using the `inja` template engine to render a custom message.
    - The `/jason` endpoint returns a predefined JSON object.
    - The `/echo` endpoint echoes the body of the incoming request.

3. **Server**:
    - The `httpxx::Server` is instantiated with a `Router` and the application configuration, then started to handle incoming requests.

### Routing System

The `Router` object is used to define endpoints for various HTTP methods (e.g., GET, POST). The `add_endpoint()` method is used to register these route handlers.

### Template Rendering with Inja

The `/param_test/` endpoint demonstrates how to parse request parameters and use the `inja` C++ template engine to render dynamic content based on user input.

### File Handling

The server can serve static files using `httpxx::handlers::serve_file()`, which reads the files from the file system and returns them as HTTP responses.

### Dependencies

- `nlohmann_json` for handling JSON data.
- `inja` for template rendering.
- `fmt` for formatting strings.

### Project Setup

### Prerequisites

- A C++20-compatible compiler (e.g., GCC 13 or later).
- CMake 3
- A TOML configuration file (`config.toml`) for server settings (e.g., port and file path).

### Build Instructions

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

### Config File Example (`config.toml`)

Here is an example of what your `config.toml` file might look like:

```toml
[app]
port = 8080
www_path = "/path/to/static/files"
```

### Dependencies:

- `nlohmann_json` for JSON handling.
- `inja` for template rendering.
- `fmt` for string formatting.

Tests are located in the `tests/` directory.

## Contributing

Contributions to `httpxx` are welcome! If you'd like to contribute, feel free to fork the repository and submit a pull request. Please ensure your changes are well-documented and tested.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
