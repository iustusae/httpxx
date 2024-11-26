
#include <functional>
#include <string>

template <class ReturnType, class... ParameterTypes>
using fn_handler_t = std::function<Return(... ParameterTypes)>;
using endpoint_t = std::unordered_map<std::string, std::function<void(void)>>;

class RequestHandler {

  void lewl() {
    endpoint_t e{};
    auto r = e.at("a");
  }
};
