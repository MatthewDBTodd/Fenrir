#pragma once

#include "direction.h"
#include "types.h"

#include <array>
#include <cstdint>

namespace piece {

constexpr std::array<std::uint64_t, NUM_SQUARES> 
generate_knight_att_squares() noexcept {
    std::array<std::uint64_t, NUM_SQUARES> att_squares;
    std::uint64_t mask { 1ull };
    for (std::size_t i = A1; i < NUM_SQUARES; ++i) {
        const auto squares {
              direction::north_north_east(mask) 
            | direction::north_north_west(mask)
            | direction::south_south_east(mask)
            | direction::south_south_west(mask)
            | direction::north_east_east(mask)
            | direction::north_west_west(mask)
            | direction::south_east_east(mask)
            | direction::south_west_west(mask)
        };
        att_squares[i] = squares;

        mask <<= 1;
    }
    return att_squares;
}

} // namespace piece
