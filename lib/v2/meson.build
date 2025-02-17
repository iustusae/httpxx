# Collect header files for the library
httpxx_sources = files(
  './httpxx/configuration.hh',
  './httpxx/endpoint.hh',
  './httpxx/enums.hh',
  './httpxx/httpxx_assert.hh',
  './httpxx/objects.hh',
  './httpxx/request_handlers.hh',
  './httpxx/router.hh',
  './httpxx/router.hh',
  './httpxx/server.hh',
  './httpxx/socket.hh',
  './httpxx/socket_enums.hh',
)

# Create the static library
httpxx_lib = static_library(
  'httpxx_v2',
  httpxx_sources,
  include_directories: include_directories('..'),
)

# Install the library and public headers
install_headers(httpxx_sources, subdir: 'httpxx')

# For `pkg-config`, meson handles it automatically if you add the following:
pkg = import('pkgconfig')
pkg.generate(
  httpxx_lib,
  name: 'httpxx',
  description: 'A C++ HTTP library',
  version: meson.project_version(),
)
