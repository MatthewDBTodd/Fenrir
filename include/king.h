#pragma once

#include "direction.h"
#include "types.h"

#include <array>
#include <cstdint>

namespace piece {

constexpr std::array<std::uint64_t, NUM_SQUARES> 
generate_king_att_squares() noexcept {
    std::array<std::uint64_t, NUM_SQUARES> att_squares;
    std::uint64_t mask { 1 };
    for (std::size_t i = A1; i < NUM_SQUARES; ++i) {
        auto squares { 
            mask | direction::east(mask) | direction::west(mask) 
        };
        squares |= direction::north(squares) | direction::south(squares);
        squares ^= mask;
        att_squares[i] = squares;

        mask <<= 1;
    }
    return att_squares;
}

} // namespace piece
