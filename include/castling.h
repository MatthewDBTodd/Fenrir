#pragma once

#include "decoded_move.h"
#include "types.h"

#include "fenrir_assert.h"
#include <cstdint>
#include <optional>
#include <string_view>

class CastlingRights {
public:
    CastlingRights() = default;

    static std::optional<CastlingRights> from_fen(std::string_view castling);

    bool can_castle(const Colour colour, const Piece piece) const {
        return castling & mask(colour, piece);
    }

    void update_castling(const DecodedMove &move);

    void operator()(const move_type_v::Quiet &quiet);
    void operator()(const move_type_v::Capture &cap);
    void operator()(const move_type_v::CastleKingSide &cks);
    void operator()(const move_type_v::CastleQueenSide &cqs);
    void operator()(const move_type_v::CapturePromotion &cp);

    // remaining move types can't affect castling rights
    void operator()(auto&&) {}
    // void operator()(const move_type_v::DoublePawnPush&) {}
    // void operator()(const move_type_v::EnPassant&) {}
    // void operator()(const move_type_v::MovePromotion&) {}

private:
    explicit CastlingRights(const std::uint8_t mask) :
        castling(mask)
    {}

    void invalidate(const Colour colour, const Piece piece) {
        castling &= ~mask(colour, piece);
    }

    void invalidate_both(const Colour colour) {
        invalidate(colour, KING);
        invalidate(colour, QUEEN);
    }

    static std::uint8_t mask(const Colour colour, const Piece piece) {
        BOOST_ASSERT(colour != NUM_COLOURS);
        BOOST_ASSERT(piece == KING || piece == QUEEN);
        // bit ugly but avoids branches
        return 1 << ((colour * 2) + (piece - QUEEN));
    }

    bool can_castle(const Colour colour) const {
        BOOST_ASSERT(colour != NUM_COLOURS);
        return (0b11 << (colour * 2)) & castling;
    }

    void check_capture(const Square dest_square);

    std::uint8_t castling { 0b1111 };
};