#include <chrono>
#include <filesystem>
#include <fstream>
#include <httpxx/configuration.hh>
#include <httpxx/request_handlers.hh>
#include <httpxx/router.hh>
#include <include/inja.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <random>
#include <string>
#include <vector>

#include "httpxx/server.hh"

// Simple in-memory task store to demonstrate concurrent access and JSON APIs.
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
  std::vector<Task> tasks_;
  std::mt19937 rng_{std::random_device{}()};

  std::string generate_id() {
    static const char* chars = "0123456789abcdef";
    std::uniform_int_distribution<> d(0, 15);
    std::string id;
    id.reserve(8);
    for (int i = 0; i < 8; ++i) id += chars[d(rng_)];
    return id;
  }

  static std::string now_iso() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return std::string(buf);
  }

 public:
  nlohmann::json add_task(const nlohmann::json& j) {
    std::lock_guard<std::mutex> g(mutex_);
    Task t{generate_id(),
           j.value("title", "untitled"),
           j.value("description", ""),
           j.value("status", "pending"),
           now_iso(),
           j.value("tags", std::vector<std::string>{})};
    tasks_.insert(tasks_.begin(), std::move(t));
    return nlohmann::json{{"id", tasks_.front().id},
                          {"title", tasks_.front().title},
                          {"description", tasks_.front().description},
                          {"status", tasks_.front().status},
                          {"created_at", tasks_.front().created_at},
                          {"tags", tasks_.front().tags}};
  }

  nlohmann::json list_tasks() {
    std::lock_guard<std::mutex> g(mutex_);
    nlohmann::json arr = nlohmann::json::array();
    for (auto const& t : tasks_) {
      arr.push_back({{"id", t.id},
                     {"title", t.title},
                     {"description", t.description},
                     {"status", t.status},
                     {"created_at", t.created_at},
                     {"tags", t.tags}});
    }
    return arr;
  }

  bool update_task(const std::string& id, const nlohmann::json& upd) {
    std::lock_guard<std::mutex> g(mutex_);
    for (auto& t : tasks_) {
      if (t.id == id) {
        if (upd.contains("title")) t.title = upd["title"].get<std::string>();
        if (upd.contains("description"))
          t.description = upd["description"].get<std::string>();
        if (upd.contains("status")) t.status = upd["status"].get<std::string>();
        if (upd.contains("tags"))
          t.tags = upd["tags"].get<std::vector<std::string>>();
        return true;
      }
    }
    return false;
  }

  bool delete_task(const std::string& id) {
    std::lock_guard<std::mutex> g(mutex_);
    auto it = std::remove_if(tasks_.begin(), tasks_.end(),
                             [&](const Task& t) { return t.id == id; });
    if (it == tasks_.end()) return false;
    tasks_.erase(it, tasks_.end());
    return true;
  }
};

// Helper: safely read a file under a root directory (prevents path traversal)
static bool safe_read_file(const std::filesystem::path& root,
                           const std::string& requested,
                           std::string& out_content, std::string& out_type) {
  std::filesystem::path p = requested;
  // prevent absolute path and path traversal
  if (p.is_absolute()) return false;
  for (auto const& part : p) {
    if (part == "..") return false;
  }
  std::filesystem::path full = root / p;
  if (!std::filesystem::exists(full) || !std::filesystem::is_regular_file(full))
    return false;

  std::ifstream ifs(full, std::ios::binary);
  if (!ifs) return false;
  out_content.assign((std::istreambuf_iterator<char>(ifs)),
                     std::istreambuf_iterator<char>());

  // minimal content-type mapping
  auto ext = full.extension().string();
  if (ext == ".html")
    out_type = "text/html";
  else if (ext == ".css")
    out_type = "text/css";
  else if (ext == ".js")
    out_type = "application/javascript";
  else if (ext == ".json")
    out_type = "application/json";
  else if (ext == ".png")
    out_type = "image/png";
  else if (ext == ".jpg" || ext == ".jpeg")
    out_type = "image/jpeg";
  else
    out_type = "application/octet-stream";

  return true;
}

int main() {
  try {
    // Load configuration (example/config.toml)
    auto config = httpxx::Config::fromFile("./config.toml");

    auto tasks = std::make_shared<TaskManager>();

    // Where static files live for this example
    std::filesystem::path static_root =
        std::filesystem::current_path() / "example" / "static";

    // Simple inja template for /hello/:name
    inja::Environment env;
    const std::string hello_tmpl = R"(
      <!doctype html>
      <html><head><meta charset="utf-8"><title>Hello</title></head>
      <body>
        <h1>Hello {{ name | escape }}</h1>
        <p>Server time: {{ time }}</p>
      </body>
      </html>)";

    auto router =
        httpxx::RouterBuilder()
            // Serve a minimal SPA root
            .get(
                "/",
                [](const httpxx::Request&) {
                  return httpxx::ResponseBuilder::ok()
                      .contentType("text/html")
                      .body(
                          R"(<!doctype html><html><head><meta charset="utf-8"><title>HTTPXX Demo</title></head><body><h1>HTTPXX Demo</h1><p>Open the API at <code>/api/tasks</code></p></body></html>)")
                      .build();
                })

            // Static file endpoint (example/static/<file>)
            .get("/static/:file",
                 [static_root](const httpxx::Request& req) {
                   try {
                     auto file = req.request_parameters.at(":file");
                     std::string content, ctype;
                     if (!safe_read_file(static_root, file, content, ctype)) {
                       return httpxx::ResponseBuilder::notFound()
                           .contentType("text/plain")
                           .body("Not found")
                           .build();
                     }
                     return httpxx::ResponseBuilder::ok()
                         .contentType(ctype)
                         .body(content)
                         .build();
                   } catch (...) {
                     return httpxx::ResponseBuilder::badRequest()
                         .contentType("text/plain")
                         .body("Bad request")
                         .build();
                   }
                 })

            // Hello template: /hello/:name
            .get("/hello/:name",
                 [&env, &hello_tmpl](const httpxx::Request& req) {
                   try {
                     auto name = req.request_parameters.at(":name");
                     nlohmann::json data;
                     data["name"] = name;
                     data["time"] = std::chrono::system_clock::to_time_t(
                         std::chrono::system_clock::now());
                     auto rendered = env.render(hello_tmpl, data);
                     return httpxx::ResponseBuilder::ok()
                         .contentType("text/html")
                         .body(rendered)
                         .build();
                   } catch (...) {
                     return httpxx::ResponseBuilder::badRequest()
                         .contentType("text/plain")
                         .body("Bad request")
                         .build();
                   }
                 })

            // JSON API: list tasks
            .get("/api/tasks",
                 [tasks](const httpxx::Request&) {
                   return httpxx::ResponseBuilder::ok()
                       .json(tasks->list_tasks())
                       .build();
                 })

            // Create a task (POST body must be JSON)
            .post(
                "/api/tasks",
                [tasks](const httpxx::Request& req) {
                  try {
                    auto body = req.body.value_or("{}");
                    auto j = nlohmann::json::parse(body);
                    auto created = tasks->add_task(j);
                    return httpxx::ResponseBuilder::ok().json(created).build();
                  } catch (const std::exception& e) {
                    return httpxx::ResponseBuilder::badRequest()
                        .json({{"error", e.what()}})
                        .build();
                  }
                })

            // Update task
            .put("/api/tasks/:id",
                 [tasks](const httpxx::Request& req) {
                   try {
                     auto id = req.request_parameters.at(":id");
                     auto body = req.body.value_or("{}");
                     auto j = nlohmann::json::parse(body);
                     if (tasks->update_task(id, j)) {
                       return httpxx::ResponseBuilder::ok()
                           .json({{"status", "ok"}})
                           .build();
                     } else {
                       return httpxx::ResponseBuilder::notFound()
                           .json({{"error", "not found"}})
                           .build();
                     }
                   } catch (const std::exception& e) {
                     return httpxx::ResponseBuilder::badRequest()
                         .json({{"error", e.what()}})
                         .build();
                   }
                 })

            // Delete task
            .del("/api/tasks/:id",
                 [tasks](const httpxx::Request& req) {
                   try {
                     auto id = req.request_parameters.at(":id");
                     if (tasks->delete_task(id)) {
                       return httpxx::ResponseBuilder::ok()
                           .json({{"status", "deleted"}})
                           .build();
                     } else {
                       return httpxx::ResponseBuilder::notFound()
                           .json({{"error", "not found"}})
                           .build();
                     }
                   } catch (...) {
                     return httpxx::ResponseBuilder::badRequest()
                         .json({{"error", "bad request"}})
                         .build();
                   }
                 })

            // Echo endpoint to test large bodies
            .post("/api/echo",
                  [](const httpxx::Request& req) {
                    return httpxx::ResponseBuilder::ok()
                        .contentType("text/plain")
                        .body(req.body.value_or(""))
                        .build();
                  })
            .build();

    // Informational log
    std::clog << "Starting HTTPXX example server at port " << config.getPort()
              << ", static root: " << static_root.string() << '\n';

    // Create and run the server (blocks until stopped by the library)
    auto server = httpxx::Server(std::move(router), std::move(config));
    server.start();

  } catch (const std::exception& e) {
    std::cerr << "Fatal server error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}
