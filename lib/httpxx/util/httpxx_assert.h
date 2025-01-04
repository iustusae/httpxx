#pragma once

#include <fmt/format.h>

#include <iostream>
#include <ostream>
#include <source_location>
#include <string_view>

namespace httpxx {
inline void httpxx_assert(
    bool expected, const std::string_view message,
    const std::source_location& location = std::source_location::current()) {
  if (expected) return;
  std::clog << fmt::format("[{}:{}] - {}(): ", location.file_name(),
                           location.line(), location.function_name())
            << message << std::endl;
  std::abort();
}
}  // namespace httpxx
