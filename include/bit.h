#pragma once

#include <cstdint>

namespace utility::bit {

constexpr std::uint64_t lowest_set_bit(const std::uint64_t n) noexcept {
    return n & -n;
}

constexpr std::uint64_t clear_lowest_bit(const std::uint64_t n) noexcept {
    return n & (n-1);
}

} // utility::bit