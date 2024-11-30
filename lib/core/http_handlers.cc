#include "http_handlers.hh"
#include "http_headers.hh"
#include "http_objects.hh"
#include <cstring>
#include <sstream>
#include <unistd.h>
namespace httpxx {
inline float extractHttpVersion(const std::string &http_version) {
  std::string r{};
  float res{};

  for (const auto &c : http_version) {
    if (std::isdigit(c) || c == '.') {
      r += c;
    }
  }

  try {
    res = std::stof(r);
  } catch (std::exception &e) {
    // TODO: answer with a server error.
  }

  return res;
}

inline std::vector<std::string> split(const std::string &str,
                                      const std::string &delimiter) {
  auto s = str;
  std::vector<std::string> tokens;
  size_t pos = 0;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    std::string token = s.substr(0, pos);
    token.erase(token.begin(),
                std::ranges::find_if(token, [](const unsigned char ch) {
                  return !std::isspace(ch);
                }));
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);

  return tokens;
}

namespace handlers {

inline Request createRequest(const char *_request_string) {
  RequestLine rl{};

  std::istringstream iss(_request_string);
  std::string line{};
  std::getline(iss, line);
  assert(line.find("HTTP") != std::string::npos);

  std::istringstream intrss(line);
  std::vector<std::string> headers;
  while (std::getline(intrss, line, ' ')) {
    headers.push_back(line);
  }
  assert(headers.size() == 3);

  rl.method = stringToHttpMethod(headers.at(0));
  rl.uri = headers.at(1);
  rl.http_version = extractHttpVersion(headers.at(2));

  Headers hd{httpxx::headers};
  while (std::getline(iss, line)) {
    // Trim whitespace and check for empty line or just CR/LF
    if (line.empty() || line == "\r") {
      break;
    }

    std::istringstream hss(line);

    auto hv = split(line, ":");
    const auto &head = hv.at(0);
    auto value = hv.at(1);
    assert(not value.empty());

    hd.at(head) = std::make_optional(value);
  }

  // for (const auto& [k, v]: hd) {
  //
  //     if (v.has_value()) {
  //
  //     std::clog << k << ":" << v.value() << "\n";
  //     }
  //
  // }

  std::getline(iss, line);
  Body body{line};

  auto rq = Request{rl, hd, std::make_optional(body)};
  return rq;
}

void response_write(const Response &response, const int client_fd) {
  write(client_fd, response.toString().c_str(),
        std::strlen(response.toString().c_str()));
}

void handle_request(const httpxx::Router &router, int client_fd,
                    const char *buffer) {
  const auto req = createRequest(buffer);
  auto resp = router.get_handler_fn(req.request_line.uri)(client_fd, req);
  response_write(resp, client_fd);
  close(client_fd);
}

} // namespace handlers

} // namespace httpxx
