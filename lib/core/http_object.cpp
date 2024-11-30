
#include "http_objects.hh"
namespace httpxx {

inline std::ostream &operator<<(std::ostream &out, const RequestLine &rl) {
  out << "RequestLine {" << rl.method << ", " << rl.uri << ", "
      << rl.http_version << "}";

  return out;
}
} // namespace httpxx
