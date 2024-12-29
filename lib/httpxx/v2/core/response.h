//
// Created by iustus on 12/21/24.
//

#ifndef RESPONSE_H
#define RESPONSE_H
#include <bits/chrono_io.h>
#include <httpxx/util/httpxx_assert.h>

#include <ranges>
#include <utility>
#include <vector>

#include "httpxx/core/http_enums.hh"
namespace httpxx::v2 {
using params_t = std::unordered_map<std::string, std::string> ;

struct Request {
  HttpMethod method{};
  std::string uri{};
  params_t request_params{};
  std::optional<std::string> body{};

  explicit Request(const char* raw_request) {
    parse(raw_request);
  }
  explicit Request(const HttpMethod method, std::string  uri): method(method), uri(std::move(uri)) {}
  Request() = default;


  bool operator==(const Request& other) const {
    return method == other.method && uri == other.uri;
  }

private:
  void parse(const char* raw_request) {
       std::istringstream iss(raw_request);
    std::string line {};
    std::getline(iss, line);
    const auto start_line = std::ranges::views::split(line, ' ');
  }
};
}


#endif //RESPONSE_H