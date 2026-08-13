project(
  'httpxx',
  'cpp',
  version: '2.0.0',
  meson_version: '>=0.60.0',
  default_options: [
    'O3',
    'cpp_std=c++20',         # Cross-platform C++20 flag handling
    'buildtype=release',      # Default build type
    'warning_level=3',        # Enables -Wall, -Wextra, -Wpedantic
    'b_lto=true',             # Link-time optimization for release
    'gnu_symbol_visibility=hidden'
  ]
)

# External dependencies
fmt_dep = dependency('fmt', required: true)

# Process library directory
subdir('lib/v2')

# Create example application executable
example = executable(
  'example',
  'example/main.cc',
  dependencies: [fmt_dep, httpxx_dep]
)
