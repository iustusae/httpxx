#pragma once

#include <fmt/format.h>
#include <netinet/in.h>

#include <filesystem>
#include <string>
#include <utility>

#include "../util/httpxx_assert.h"
#include "include/tomlpp.hh"

struct Config {
  in_port_t _port{};
  std::string _www_path{};

  [[deprecated(" use #include <httpxx/configuration.hh>")]]
  Config() = default;

  [[deprecated(" use #include <httpxx/configuration.hh>")]]
  Config(const in_port_t port, std::string www_path)
      : _port(port) {
    httpxx::httpxx_assert(std::filesystem::exists(www_path),
                          fmt::format("{} does not exist.", www_path));
    _www_path = std::move(www_path);
  }
  [[deprecated(" use #include <httpxx/configuration.hh>")]]

  static Config fromFile(const std::string& path) {
    httpxx::httpxx_assert(std::filesystem::exists(path) == true,
                          fmt::format("File '{}' does not exist", path));

    toml::table table = toml::parse_file(path);
    Config config{};

    config._port = *table["server"]["port"].value<in_port_t>();
    config._www_path = *table["server"]["www_path"].value<std::string>();

    httpxx::httpxx_assert(
        std::filesystem::exists(config._www_path),
        fmt::format("www_path '{}' does not exist", config._www_path));

    return config;
  }

  [[deprecated(" use #include <httpxx/configuration.hh>")]]
  friend bool operator==(const Config& lhs, const Config& rhs) {
    return lhs._port == rhs._port and lhs._www_path == rhs._www_path;
  }
};
