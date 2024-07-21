#pragma once

#include "bitboard.h"

#include <cstdint>

class Board {
private:
    Bitboard bitboard; // 64 
    std::uint16_t total_half_moves {}; // half moves since the start of the game
    std::uint16_t quiet_half_moves {}; // half moves since the last capture/pawn move
    std::uint8_t colour : 1;
    std::uint8_t castling : 4;
};