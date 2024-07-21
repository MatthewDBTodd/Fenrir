#pragma once

#include <vector>
#include <string_view>

namespace utility {

std::vector<std::string_view> split(std::string_view s, const char c);

} // namespace utility