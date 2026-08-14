# httpxx

minimal c++ http server. header-only core under `lib/v2/httpxx/`, built with meson.

## what it does

parses raw http/1.1 bytes off a tcp socket, routes to a handler, builds a response, sends it back.

## deps

- toml++ - vendored, parses config.toml
- inja - vendored, template rendering
- nlohmann_json + fmt - system deps, json + formatting in the handlers

## build
``` bash
meson setup build
meson compile -C build
```
root build pulls in the v2 lib build and builds the example app against it.

## run

./example_server.sh

reads example/config.toml, spins up the server, serves whatever endpoints main.cc registered plus static files off example/static/.

## how a request actually flows

tcp client sends raw http/1.1 bytes, socket parses it into a request object, server hands it to the router, router matches method/path against a registered endpoint, endpoint calls its handler, handler builds a response (json via nlohmann_json/fmt, a rendered inja template, or a static file), response gets serialized back through the socket, client gets bytes back.

## roadmap

not in order of "cool factor," in order of what actually has to happen first.

### separation of concerns
- `socket.hh` right now mixes raw transport (send/recv) with http/1.1 framing. split those. transport layer shouldnt know or care what protocol is riding on top of it — this is the actual blocker for http/2, not a nice-to-have.
- `request_handlers.hh` is doing three jobs: response building, json serialization, template rendering, static file serving. split into a thin handler api + pluggable renderers (json, inja, static-file) behind one common response-writing interface.
- keep toml++ parsing isolated from `configuration.hh`'s in-memory model. dont let the parser leak into the rest of the codebase.

### tests
- unit tests per component: socket/parsing (malformed requests, partial reads, chunked bodies), router matching (method/path edge cases), config parsing.
- integration test that spins up the example server and hits it over real sockets, not mocked.
- fuzz the http/1.1 parser once its split out of the transport layer. thats the part thatll actually get hit with garbage input in the wild.

### http/2
- needs the socket/framing split above done first. h2 is a binary framing layer over the same transport, not a rewrite of the transport.
- build order: binary framing → hpack header compression → stream multiplexing. dont try to do all three at once.
- tls is basically mandatory here too, real clients wont negotiate h2 over plaintext outside internal testing. need alpn for the protocol upgrade.

### async i/o
- current model reads like blocking/synchronous per the arch. h2 multiplexing barely matters if one connection blocks the thread. look at epoll (or io_uring if going linux-only) before or alongside the h2 work, not after.

### robustness
- keep-alive / connection reuse for http/1.1 — not clear if this exists yet, check before assuming.
- timeouts on slow/idle connections. malformed requests shouldnt take the server down.
- graceful shutdown.

### benchmarks
- throughput/latency under load, before and after each phase above. the whole pitch of this project is "no framework bloat"
