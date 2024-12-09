#pragma once

#include <filesystem>
#include <netinet/in.h>
#include <string>
#include <utility>

#include "tomlpp.hh"
#include "../util/httpxx_assert.h"


struct Config {
  in_port_t _port{};
  std::string _www_path{};

  Config() = default;

  Config(const in_port_t port, std::string www_path) : _port(port) {
    assert(std::filesystem::exists(www_path));
    _www_path = std::move(www_path);
  }

  static Config fromFile(const std::string& path) {
    httpxx_assert(std::filesystem::exists(path) == true,
                  std::format("File '{}' does not exist", path));

    toml::table table = toml::parse_file(path);
    Config config{};

    config._port = *table["server"]["port"].value<in_port_t>();
    config._www_path = *table["server"]["www_path"].value<std::string>();

    httpxx_assert(std::filesystem::exists(config._www_path),
                  std::format("www_path '{}' does not exist",
                              config._www_path));

    return config;
  }


  friend bool operator==(const Config& lhs, const Config& rhs) {
    return lhs._port == rhs._port and lhs._www_path == rhs._www_path;
  }
};