// #include <httpxx/configuration.hh>
// #include <httpxx/request_handlers.hh>
// #include <httpxx/router.hh>
// #include <include/inja.hpp>
// #include <nlohmann/json.hpp>
//
// #include "httpxx/server.hh"
//
// int main() {
//   try {
//     // Load configuration from file
//     auto config = httpxx::Config::fromFile("./config.toml");
//
//     // Create router using builder pattern
//     auto router =
//         httpxx::RouterBuilder()
//             // Basic endpoint demonstrating GET and POST handling
//             .methods("/endpoint",
//                      {httpxx::HttpMethod::GET, httpxx::HttpMethod::POST},
//                      [](const httpxx::Request& req) {
//                        if (req.method == httpxx::HttpMethod::GET) {
//                          return httpxx::ResponseBuilder::ok()
//                              .contentType("text/plain")
//                              .body("Oh, a GET request!")
//                              .build();
//                        }
//
//                        // Handle POST
//                        return httpxx::ResponseBuilder::ok()
//                            .html(fmt::format(
//                                "<h1>Oh, a POST request!</h1> {}",
//                                req.body.value_or("request has no body")))
//                            .build();
//                      })
//
//             // Serve index.html from configured www path
//             .get("/",
//                  [config](const httpxx::Request&) {
//                    return httpxx::FileServer::serveFile(
//                        std::filesystem::path(config.getWwwPath()) /
//                        "index.html");
//                  })
//
//             // Parameter test endpoint with template rendering
//
//             // JSON response example
//             .get("/jason",
//                  [](const httpxx::Request&) {
//                    nlohmann::json data = {
//                        {"string_id", "unique12345"},
//                        {"content", "Lorem ipsum dolor sit amet..."},
//                        {"metadata",
//                         {{"author", "John Doe"},
//                          {"created_at", "2024-12-01T10:00:00Z"},
//                          {"version", 1.0},
//                          {"tags", {"sample", "text", "long"}}}},
//                        {"properties",
//                         {{"length", 350},
//                          {"encoding", "UTF-8"},
//                          {"is_encrypted", false}}}};
//
//                    return httpxx::ResponseBuilder::ok().json(data).build();
//                  })
//
//             // Echo endpoint
//             .get("/echo",
//                  [](const httpxx::Request& req) {
//                    return httpxx::ResponseBuilder::ok()
//                        .html(req.body.value_or(""))
//                        .build();
//                  })
//             .get(
//                 "/param_test",
//                 [](const httpxx::Request& request) {
//                   if (request.request_parameters.empty()) {
//                     return httpxx::ResponseBuilder::badRequest()
//                         .html(
//                             "<h1>Error: Please provide name and email "
//                             "parameters</h1>")
//                         .build();
//                   }
//
//                   try {
//                     inja::Environment env;
//                     std::string tmpl = R"(
//         <html>
//         <body>
//             <h1>User Information</h1>
//             {% if name %}
//             <p>Name: {{ name }}</p>
//             {% endif %}
//             {% if email %}
//             <p>Email: {{ email }}</p>
//             {% endif %}
//             <h2>All Parameters:</h2>
//             <ul>
//             {% for param in request_parameters %}
//                 <li>{{ param.first }}: {{ param.second }}</li>
//             {% endfor %}
//             </ul>
//         </body>
//         </html>
//         )";
//
//                     // Convert request parameters to nlohmann::json
//                     nlohmann::json data;
//                     for (const auto& [key, value] :
//                          request.request_parameters) {
//                       data[key] = value;
//                     }
//
//                     // Add the full parameters map (as a JSON object) for
//                     // iteration in the template
//                     nlohmann::json parameters;
//                     for (const auto& [key, value] :
//                          request.request_parameters) {
//                       parameters.push_back({{"first", key}, {"second",
//                       value}});
//                     }
//                     data["request_parameters"] = parameters;
//
//                     // Render the template
//                     auto rendered = env.render(tmpl, data);
//
//                     return
//                     httpxx::ResponseBuilder::ok().html(rendered).build();
//                   } catch (const std::exception& e) {
//                     return httpxx::ResponseBuilder{}
//                         .html(fmt::format("<h1>Template Error</h1><p>{}</p>",
//                                           e.what()))
//                         .build();
//                   }
//                 })
//             .build();
//
//     // Log server info
//     std::clog << fmt::format(
//                      "Server running at: http://localhost:{}, www_path = {}",
//                      config.getPort(), config.getWwwPath().string())
//               << '\n';
//
//     // Create and start server
//     auto server = httpxx::Server(std::move(router), std::move(config));
//     server.start();
//
//   } catch (const httpxx::ConfigError& e) {
//     std::cerr << "Configuration error: " << e.what() << '\n';
//     return 1;
//   } catch (const std::exception& e) {
//     std::cerr << "Server error: " << e.what() << '\n';
//     return 1;
//   }
//
//   return 0;
// }
//

#include <chrono>
#include <deque>
#include <httpxx/configuration.hh>
#include <httpxx/request_handlers.hh>
#include <httpxx/router.hh>
#include <include/inja.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <random>

#include "httpxx/server.hh"

// In-memory task management system
class TaskManager {
 private:
  struct Task {
    std::string id;
    std::string title;
    std::string description;
    std::string status;
    std::string created_at;
    std::vector<std::string> tags;
  };

  std::mutex mutex_;
  std::deque<Task> tasks_;
  std::mt19937 rng_{std::random_device{}()};

  std::string generate_id() {
    const std::string chars = "0123456789abcdef";
    std::string id;
    for (int i = 0; i < 8; ++i) {
      id += chars[std::uniform_int_distribution<>(0, chars.size() - 1)(rng_)];
    }
    return id;
  }

 public:
  nlohmann::json add_task(const nlohmann::json& task_data) {
    std::lock_guard<std::mutex> lock(mutex_);

    Task task{generate_id(),
              task_data["title"],
              task_data["description"],
              task_data.value("status", "pending"),
              std::to_string(
                  std::chrono::system_clock::now().time_since_epoch().count()),
              task_data.value("tags", std::vector<std::string>{})};

    tasks_.push_front(task);

    return {{"id", task.id},
            {"title", task.title},
            {"description", task.description},
            {"status", task.status},
            {"created_at", task.created_at},
            {"tags", task.tags}};
  }

  nlohmann::json get_tasks() {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json result = nlohmann::json::array();
    for (const auto& task : tasks_) {
      result.push_back({{"id", task.id},
                        {"title", task.title},
                        {"description", task.description},
                        {"status", task.status},
                        {"created_at", task.created_at},
                        {"tags", task.tags}});
    }
    return result;
  }

  bool update_task(const std::string& id, const nlohmann::json& update_data) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find_if(tasks_.begin(), tasks_.end(),
                           [&id](const Task& task) { return task.id == id; });

    if (it == tasks_.end()) return false;

    if (update_data.contains("title")) it->title = update_data["title"];
    if (update_data.contains("description"))
      it->description = update_data["description"];
    if (update_data.contains("status")) it->status = update_data["status"];
    if (update_data.contains("tags"))
      it->tags = update_data["tags"].get<std::vector<std::string>>();

    return true;
  }

  bool delete_task(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find_if(tasks_.begin(), tasks_.end(),
                           [&id](const Task& task) { return task.id == id; });

    if (it == tasks_.end()) return false;

    tasks_.erase(it);
    return true;
  }
};

int main() {
  try {
    auto config = httpxx::Config::fromFile("./config.toml");
    auto task_manager = std::make_shared<TaskManager>();

    auto router =
        httpxx::RouterBuilder()
            // Serve the SPA
            .get(
                "/",
                [](const httpxx::Request&) {
                  return httpxx::ResponseBuilder::ok()
                      .contentType("text/html")
                      .body(
                          R"(<!DOCTYPE html><html><head><meta charset="UTF-8"></head><body><div id="app"></div></body></html>)")
                      .build();
                })

            // API Endpoints
            .get("/api/tasks",
                 [task_manager](const httpxx::Request&) {
                   return httpxx::ResponseBuilder::ok()
                       .json(task_manager->get_tasks())
                       .build();
                 })

            .post(
                "/api/tasks",
                [task_manager](const httpxx::Request& req) {
                  try {
                    auto task_data =
                        nlohmann::json::parse(req.body.value_or("{}"));
                    auto new_task = task_manager->add_task(task_data);
                    return httpxx::ResponseBuilder::ok().json(new_task).build();
                  } catch (const std::exception& e) {
                    return httpxx::ResponseBuilder::badRequest()
                        .json({{"error", e.what()}})
                        .build();
                  }
                })

            .put("/api/tasks/:id",
                 [task_manager](const httpxx::Request& req) {
                   try {
                     auto id = req.request_parameters.at(":id");
                     auto update_data =
                         nlohmann::json::parse(req.body.value_or("{}"));

                     if (task_manager->update_task(id, update_data)) {
                       return httpxx::ResponseBuilder::ok()
                           .json({{"status", "success"}})
                           .build();
                     }
                     return httpxx::ResponseBuilder::notFound()
                         .json({{"error", "Task not found"}})
                         .build();
                   } catch (const std::exception& e) {
                     return httpxx::ResponseBuilder::badRequest()
                         .json({{"error", e.what()}})
                         .build();
                   }
                 })

            .del("/api/tasks/:id",
                 [task_manager](const httpxx::Request& req) {
                   auto id = req.request_parameters.at(":id");
                   if (task_manager->delete_task(id)) {
                     return httpxx::ResponseBuilder::ok()
                         .json({{"status", "success"}})
                         .build();
                   }
                   return httpxx::ResponseBuilder::notFound()
                       .json({{"error", "Task not found"}})
                       .build();
                 })
            .build();

    std::clog << fmt::format("SPA server running at: http://localhost:{}",
                             config.getPort())
              << '\n';

    auto server = httpxx::Server(std::move(router), std::move(config));
    server.start();

  } catch (const std::exception& e) {
    std::cerr << "Server error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
