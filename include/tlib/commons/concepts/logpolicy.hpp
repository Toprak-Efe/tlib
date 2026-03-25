#include <source_location>
#include <string_view>

template <typename T>
concept LogPolicy = requires(T t, std::string_view msg, std::source_location sl) {
    t.log(msg, sl);
};
