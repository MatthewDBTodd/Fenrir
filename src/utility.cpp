#include "utility.h"

#include <algorithm>

namespace utility {

std::vector<std::string_view> split(std::string_view s, const char c) {
    std::vector<std::string_view> rv;
    auto start { s.begin() };
    while (true) {
        const auto boundary { std::find(start, s.end(), c) };
        // don't push an empty string if there's nothing between
        if (boundary - start+1 > 1) {
            rv.emplace_back(start, boundary);
        }
        if (boundary == s.end()) {
            break;
        }
        start = boundary + 1;
    }
    return rv;
}

} // namespace utility