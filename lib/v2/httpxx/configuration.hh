#pragma once

#include <fmt/format.h>
#include <netinet/in.h>

#include <filesystem>
#include <include/tomlpp.hh>
#include <iostream>

namespace httpxx {

class ConfigError : public std::runtime_error {
 public:
  explicit ConfigError(const std::string& message)
      : std::runtime_error(message) {}
};

class Config {
 public:
  Config() = default;

  Config(in_port_t port, std::filesystem::path www_path)
      : port_(port), www_path_(std::move(www_path)) {
    validateWwwPath();
  }

  static Config fromFile(const std::filesystem::path& path) {
    validateConfigFile(path);

    try {
      auto table = toml::parse_file(path.string());

      Config config;
      config.port_ = getRequiredValue<in_port_t>(table, "server", "port");
      config.www_path_ =
          getRequiredValue<std::string>(table, "server", "www_path");

      config.validateWwwPath();
      std::clog << fmt::format("Correctly loaded config: www_path: {}\n",
                               config.www_path_.string());
      return config;
    } catch (const toml::parse_error& e) {
      throw ConfigError(
          fmt::format("Failed to parse config file: {}", e.what()));
    }
  }

  [[nodiscard]] in_port_t getPort() const { return port_; }
  [[nodiscard]] const std::filesystem::path& getWwwPath() const {
    return www_path_;
  }

  [[nodiscard]] bool isValid() const {
    return port_ != 0 && !www_path_.empty() &&
           std::filesystem::exists(www_path_);
  }

  Config& setPort(in_port_t port) {
    port_ = port;
    return *this;
  }

  Config& setWwwPath(std::filesystem::path path) {
    www_path_ = std::move(path);
    validateWwwPath();
    return *this;
  }

  friend bool operator==(const Config& lhs, const Config& rhs) {
    return lhs.port_ == rhs.port_ && lhs.www_path_ == rhs.www_path_;
  }

  friend bool operator!=(const Config& lhs, const Config& rhs) {
    return !(lhs == rhs);
  }

 private:
  in_port_t port_{0};
  std::filesystem::path www_path_;

  void validateWwwPath() const {
    if (!www_path_.empty() && !std::filesystem::exists(www_path_)) {
      throw ConfigError(
          fmt::format("WWW path '{}' does not exist", www_path_.string()));
    }
  }

  static void validateConfigFile(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
      throw ConfigError(
          fmt::format("Config file '{}' does not exist", path.string()));
    }
  }

  template <typename T>
  static T getRequiredValue(const toml::table& table,
                            const std::string& section,
                            const std::string& key) {
    auto node = table[section][key];
    if (!node) {
      throw ConfigError(
          fmt::format("Missing required config value: {}.{}", section, key));
    }

    auto value = node.value<T>();
    if (!value) {
      throw ConfigError(
          fmt::format("Invalid type for config value: {}.{}", section, key));
    }

    return *value;
  }
};

class ConfigBuilder {
 public:
  ConfigBuilder& setPort(in_port_t port) {
    config_.setPort(port);
    return *this;
  }

  ConfigBuilder& setWwwPath(std::filesystem::path path) {
    config_.setWwwPath(std::move(path));
    return *this;
  }

  Config build() {
    if (!config_.isValid()) {
      throw ConfigError("Invalid configuration");
    }
    return config_;
  }

 private:
  Config config_;
};

}  // namespace httpxx
