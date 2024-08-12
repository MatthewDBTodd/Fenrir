#pragma once

#include "direction.h"
#include "types.h"

#include <array>
#include <cstdint>
#include <ranges>

namespace direction {

using direction_fn = std::uint64_t (*)(const std::uint64_t);

constexpr std::uint64_t direction_from(direction_fn fn, const std::uint64_t pos) {
    std::uint64_t rv {};
    for (std::uint64_t n = fn(pos); n > 0; n = fn(n)) { rv |= n; }
    return rv;
}

static constexpr std::array<std::array<std::uint64_t, NUM_SQUARES>, NUM_SQUARES>
init_source_dest_masks() {
    const std::array<direction_fn, 8> direction_functions {
        north, south, east, west, north_east, north_west, south_east, south_west
    };

    std::array<std::array<std::uint64_t, NUM_SQUARES>, NUM_SQUARES> rv {};

    for (std::size_t source : std::views::iota(0, 64)) {
        for (std::size_t dest : std::views::iota(0, 64)) {
            const std::uint64_t source_mask { 1ul << source };
            const std::uint64_t dest_mask { 1ul << dest };

            for (const auto direction : direction_functions) {
                const auto mask { direction_from(direction, source_mask) };
                if (mask & dest_mask) {
                    rv[source][dest] = mask;
                    break;
                }
            }
        }
    }
    return rv;
}

// 2d array [king pos][pinned piece pos] which returns a direction mask from the king
// to the edge of the board in that direction
// you'll have the pinned pieces pseudo-legal moves, AND it with this mask to get
// the actual legal moves, pretty sure despite the mask here going potentially
// behind the pinner it's fine as no pseudo legal moves will go behind the pinner (?)
static constexpr std::array<std::array<std::uint64_t, NUM_SQUARES>, NUM_SQUARES>
SOURCE_DEST_MASKS { init_source_dest_masks() };

} // namespace direction

