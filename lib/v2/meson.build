# Public header definitions
httpxx_headers = files(
  'httpxx/configuration.hh',
  'httpxx/endpoint.hh',
  'httpxx/enums.hh',
  'httpxx/httpxx_assert.hh',
  'httpxx/objects.hh',
  'httpxx/request_handlers.hh',
  'httpxx/router.hh',
  'httpxx/server.hh',
  'httpxx/socket.hh',
  'httpxx/socket_enums.hh',
)

# Install public headers into <prefix>/include/httpxx
install_headers(httpxx_headers, subdir: 'httpxx')

# Expose header-only dependency target for internal/external build use
httpxx_dep = declare_dependency(
  include_directories: include_directories('.'),
  dependencies: [fmt_dep]
)

# Generate pkg-config file for system installation
pkg = import('pkgconfig')
pkg.generate(
  name: 'httpxx',
  description: 'A C++ HTTP framework',
  version: meson.project_version(),
  subdirs: 'httpxx'
)
