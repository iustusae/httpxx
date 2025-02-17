# Project settings
project('httpxx', 'cpp', version: '2.0.0', meson_version: '>=0.60.0')

# Set compiler options
add_project_arguments('-std=c++20', language: 'cpp')
add_project_arguments('-O3', '-DDEBUG', language: 'cpp') # General flags
add_project_arguments(
  '-g',
  '-Wall',
  '-Wextra',
  '-Wpedantic',
  language: 'cpp',
)

# Define build type (default to release if not set)
# if not buildtype
#   set_option('buildtype', 'release')
# endif

# Include directories
inc = include_directories('lib/v2')

# Find fmt dependency
fmt_dep = dependency('fmt', required: true)

# Subdirectory for lib/v2
subdir('lib/v2')

# Create the example executable
example = executable(
  'example',
  'example/main.cc',
  include_directories: [inc],
  dependencies: [fmt_dep],
  link_with: httpxx_lib,
)

# Install headers and libraries
install_headers(
  [
    './lib/v2/httpxx/configuration.hh',
    './lib/v2/httpxx/objects.hh',
    './lib/v2/httpxx/endpoint.hh',
    './lib/v2/httpxx/router.hh',
    './lib/v2/httpxx/httpxx_assert.hh',
    './lib/v2/httpxx/enums.hh',
    './lib/v2/httpxx/server.hh',
    './lib/v2/httpxx/socket_enums.hh',
    './lib/v2/httpxx/socket.hh',
    './lib/v2/httpxx/request_handlers.hh',
  ],
  subdir: 'httpxx',
)
