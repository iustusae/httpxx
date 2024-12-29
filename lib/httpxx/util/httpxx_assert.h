//
// Created by iustus on 12/6/24.
//

#ifndef HTTPXX_ASSERT_H
#define HTTPXX_ASSERT_H
#include <iostream>
#include <ostream>
#include <source_location>
#include <string_view>

inline void httpxx_assert(
    bool expected, const std::string_view message,
    const std::source_location& location = std::source_location::current()) {
  if (expected) return;
  std::clog << std::format("[{}:{}] - {}(): ", location.file_name(),
                           location.line(), location.function_name())
            << message << std::endl;
  std::abort();
}

#endif  // HTTPXX_ASSERT_H
