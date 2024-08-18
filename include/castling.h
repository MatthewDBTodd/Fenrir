#pragma once

#include "types.h"

#include "assert.h"
#include <cstdint>
#include <optional>
#include <string_view>

class CastlingRights {
public:
    CastlingRights() = default;

    static std::optional<CastlingRights> from_fen(std::string_view castling);

    bool can_castle(const Colour colour, const Piece piece) {
        return castling & mask(colour, piece);
    }

    void invalidate(const Colour colour, const Piece piece) {
        castling ^= mask(colour, piece);
    }

private:
    explicit CastlingRights(const std::uint8_t mask) :
        castling(mask)
    {}

    static std::uint8_t mask(const Colour colour, const Piece piece) {
        BOOST_ASSERT(colour != NUM_COLOURS);
        BOOST_ASSERT(piece == KING || piece == QUEEN);
        // bit ugly but avoids branches
        return 1 << ((colour * 2) + (piece - QUEEN));
    }

    std::uint8_t castling { 0b1111 };
};