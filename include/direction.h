#pragma once

#include <cstdint>
#include <limits>

#include "types.h"

namespace {

constexpr std::uint64_t NOT_A_FILE {
      std::numeric_limits<std::uint64_t>::max() 
    ^ (1ull << A1) ^ (1ull << A2) ^ (1ull << A3) ^ (1ull << A4)
    ^ (1ull << A5) ^ (1ull << A6) ^ (1ull << A7) ^ (1ull << A8)
};

constexpr std::uint64_t NOT_H_FILE {
      std::numeric_limits<std::uint64_t>::max()
    ^ (1ull << H1) ^ (1ull << H2) ^ (1ull << H3) ^ (1ull << H4)
    ^ (1ull << H5) ^ (1ull << H6) ^ (1ull << H7) ^ (1ull << H8)
};

constexpr std::uint64_t NOT_AB_FILE {
    NOT_A_FILE
    ^ (1ull << B1) ^ (1ull << B2) ^ (1ull << B3) ^ (1ull << B4)
    ^ (1ull << B5) ^ (1ull << B6) ^ (1ull << B7) ^ (1ull << B8)
};

constexpr std::uint64_t NOT_GH_FILE {
    NOT_H_FILE
    ^ (1ull << G1) ^ (1ull << G2) ^ (1ull << G3) ^ (1ull << G4)
    ^ (1ull << G5) ^ (1ull << G6) ^ (1ull << G7) ^ (1ull << G8)
};

} // namespace

namespace direction {

constexpr std::uint64_t north(const std::uint64_t mask) noexcept {
    return mask << 8;
}

constexpr std::uint64_t south(const std::uint64_t mask) noexcept {
    return mask >> 8;
}

constexpr std::uint64_t east(const std::uint64_t mask) noexcept {
    return (mask << 1) & NOT_A_FILE;
}

constexpr std::uint64_t west(const std::uint64_t mask) noexcept {
    return (mask >> 1) & NOT_H_FILE;
}

constexpr std::uint64_t north_east(const std::uint64_t mask) noexcept {
    return (mask << 9) & NOT_A_FILE;
}

constexpr std::uint64_t north_west(const std::uint64_t mask) noexcept {
    return (mask << 7) & NOT_H_FILE;
}

constexpr std::uint64_t south_east(const std::uint64_t mask) noexcept {
    return (mask >> 7) & NOT_A_FILE;
}

constexpr std::uint64_t south_west(const std::uint64_t mask) noexcept {
    return (mask >> 9) & NOT_H_FILE;
}

constexpr std::uint64_t north_north_east(const std::uint64_t mask) {
    return (mask << 17) & NOT_A_FILE;
}

constexpr std::uint64_t north_north_west(const std::uint64_t mask) {
    return (mask << 15) & NOT_H_FILE;
}

constexpr std::uint64_t south_south_east(const std::uint64_t mask) {
    return (mask >> 15) & NOT_A_FILE;
}

constexpr std::uint64_t south_south_west(const std::uint64_t mask) {
    return (mask >> 17) & NOT_H_FILE;
}

constexpr std::uint64_t north_east_east(const std::uint64_t mask) {
    return (mask << 10) & NOT_AB_FILE;
}

constexpr std::uint64_t north_west_west(const std::uint64_t mask) {
    return (mask << 6) & NOT_GH_FILE;
}

constexpr std::uint64_t south_east_east(const std::uint64_t mask) {
    return (mask >> 6) & NOT_AB_FILE;
}

constexpr std::uint64_t south_west_west(const std::uint64_t mask) {
    return (mask >> 10) & NOT_GH_FILE;
}

} // namespace direction
