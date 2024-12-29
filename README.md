# httpxx: A Simple HTTP/1.1 Server in C++23

`httpxx` is a simple HTTP/1.1 server implementation written in modern C++23. The server is capable of serving static files, handling various endpoints, and responding with JSON data. It provides a basic framework for building custom HTTP servers in C++ with minimal dependencies.

This project is mostly incomplete but functional, and it's designed to be easy to extend with additional functionality.

## Features

- Serve static files like HTML, CSS, JS, and images.
- Handle custom API endpoints that can return JSON, HTML, or other content.
- Basic routing and endpoint handling.
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

int main(int argc, char* argv[]) {
  // Load configuration from a TOML file
  const auto app = Config::fromFile("../config.toml");

  // Create a router to define endpoints
  httpxx::Router router;

  // Serve static file at root endpoint
  router.add_endpoint("/", [&](int client_fd, const httpxx::Request& request) {
    return httpxx::handlers::serve_file(
        std::format("{}/index.html", app._www_path));
  });

  // Handle custom JSON endpoint
  router.add_endpoint("/jason", [&](int client_fd, const httpxx::Request& request) {
    const std::string json_data = R"({
        "string_id": "unique12345",
        "content": "Lorem ipsum dolor sit amet, consectetur adipiscing elit...",
        "metadata": {
            "author": "John Doe",
            "created_at": "2024-12-01T10:00:00Z"
        }
    })";
    auto res = httpxx::http::ResponseBuilder{}
               .setStatusCode(httpxx::StatusCodes::OK)
               .setContentType(httpxx::ContentType::APPLICATION_JSON)
               .setBody(json_data)
               .build();
    return res;
  });

  // Handle echo endpoint
  router.add_endpoint("/echo", {[](const int& cfd, const httpxx::Request& req) {
    return httpxx::http::ResponseBuilder{}
           .setStatusCode(httpxx::StatusCodes::OK)
           .setHeader("Content-Type", "text/html")
           .setBody(req.body.value_or(""))
           .build();
  }});

  // Start the server
  std::clog << std::format("server at: http://localhost:{}, www_path = {}",
                           app._port, app._www_path)
      << '\n';

  // Start the HTTP server
  httpxx::Server{router, app}.start();
}
```

### Description of Example

1. **Configuration File**:
    - The server reads its configuration from a TOML file (e.g., `config.toml`). You can specify various parameters, such as the port number and the file directory for serving static content.

2. **Endpoints**:
    - The server has a root endpoint (`/`) that serves an `index.html` file from the configured `www_path`.
    - Another endpoint (`/jason`) responds with a JSON object, including some placeholder text and metadata.
    - The `/echo` endpoint simply echoes the request body back in the response, useful for testing.

3. **Server**:
    - The `httpxx::Server` is instantiated with a `Router` and the application configuration, then started with `.start()` to handle incoming requests.

### Routing System

- The `Router` object is used to define endpoints that respond to HTTP requests. The `add_endpoint()` method is used to add route handlers for different HTTP paths (e.g., `/`, `/jason`, `/echo`).

### File Handling

- The server uses `httpxx::handlers::serve_file()` to serve files like HTML and images. These files are read from the file system, and the server sends them as HTTP responses.

### 1M Requests Benchmark using Apache Benchmark to get a simple TODO-List app with html, css and js

![image](https://github.com/user-attachments/assets/db799b02-9571-4698-8d00-437b65d938ed)


## Project Setup

### Prerequisites

- C++23 compatible compiler (e.g., GCC 13 or later).
- CMake 3.29 or later.
- A TOML configuration file (`config.toml`) for server settings (e.g., port and file path).

### Build Instructions

1. **Clone the repository**:

   ```bash
   git clone [https://github.com/your-username/httpxx.git](https://github.com/iustusae/httpxx)
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

- `nlohmann_json`
- `inja`


Tests are located in the `tests/` directory.

## Contributing

Contributions to `httpxx` are welcome! If you'd like to contribute, feel free to fork the repository and submit a pull request. Please ensure your changes are well-documented and tested.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
