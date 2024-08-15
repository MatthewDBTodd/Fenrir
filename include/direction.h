#pragma once

#include <cstdint>
#include <limits>

#include "types.h"

namespace {

constexpr std::uint64_t NOT_A_FILE {
      std::numeric_limits<std::uint64_t>::max() 
    ^ (1ul << A1) ^ (1ul << A2) ^ (1ul << A3) ^ (1ul << A4)
    ^ (1ul << A5) ^ (1ul << A6) ^ (1ul << A7) ^ (1ul << A8)
};

constexpr std::uint64_t NOT_H_FILE {
      std::numeric_limits<std::uint64_t>::max()
    ^ (1ul << H1) ^ (1ul << H2) ^ (1ul << H3) ^ (1ul << H4)
    ^ (1ul << H5) ^ (1ul << H6) ^ (1ul << H7) ^ (1ul << H8)
};

constexpr std::uint64_t NOT_AB_FILE {
    NOT_A_FILE
    ^ (1ul << B1) ^ (1ul << B2) ^ (1ul << B3) ^ (1ul << B4)
    ^ (1ul << B5) ^ (1ul << B6) ^ (1ul << B7) ^ (1ul << B8)
};

constexpr std::uint64_t NOT_GH_FILE {
    NOT_H_FILE
    ^ (1ul << G1) ^ (1ul << G2) ^ (1ul << G3) ^ (1ul << G4)
    ^ (1ul << G5) ^ (1ul << G6) ^ (1ul << G7) ^ (1ul << G8)
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
