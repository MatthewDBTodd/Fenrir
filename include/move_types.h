#pragma once

#include <cstdint>

enum class MoveType : std::uint8_t {
    QUIET, 
    CAPTURE, 
    DOUBLE_PAWN_PUSH, 
    CASTLE_KINGSIDE, 
    CASTLE_QUEENSIDE, 
    EN_PASSANT, 
    MOVE_PROMOTION, 
    CAPTURE_PROMOTION,
    NUM_MOVE_TYPES
};
