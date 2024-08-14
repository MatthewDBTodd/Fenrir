#pragma once

#include "types.h"

#include <cstdint>

class CastlingRights {
public:
    bool can_kingside(const Colour colour) const {
        return colour == WHITE ? castling & WHITE_KINGSIDE
                               : castling & BLACK_KINGSIDE;
    }

    bool can_queenside(const Colour colour) const {
        return colour == WHITE ? castling & WHITE_QUEENSIDE
                               : castling & BLACK_QUEENSIDE;
    }
private:
    std::uint8_t castling { 0b1111 };

    static constexpr std::uint8_t WHITE_KINGSIDE { 0x1 };
    static constexpr std::uint8_t WHITE_QUEENSIDE { 0x2 };
    static constexpr std::uint8_t BLACK_KINGSIDE { 0x4 };
    static constexpr std::uint8_t BLACK_QUEENSIDE { 0x8 };
};