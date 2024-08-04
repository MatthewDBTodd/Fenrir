#pragma once

#include <array>
#include <bit>
#include <cassert>
#include <cstdint>

enum Colour : std::uint8_t {
    WHITE,
    BLACK,

    NUM_COLOURS,
};

enum Piece : std::uint8_t {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,

    NUM_PIECES,
};

enum Square : std::uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,

    NUM_SQUARES,
};

inline Square from_mask(const std::uint64_t mask) {
    assert(std::popcount(mask) == 1);
    return static_cast<Square>(std::countr_zero(mask));
}

inline std::uint64_t from_square(const Square square) {
    assert(square != NUM_SQUARES);
    return (1ul << square);
}

static constexpr std::array<Piece, NUM_PIECES> ALL_PIECES {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

static constexpr std::array<Piece, NUM_PIECES-1> CAPTURABLE_PIECES {
    QUEEN, ROOK, BISHOP, KNIGHT, PAWN
};

static constexpr std::array<Piece, 4> PROMOTION_PIECES {
    QUEEN, ROOK, BISHOP, KNIGHT
};

static constexpr std::array<Piece, NUM_PIECES-1> NON_PAWN_PIECES {
    KNIGHT, BISHOP, ROOK, QUEEN, KING
};

inline constexpr Colour opposite(const Colour colour) {
    return static_cast<Colour>((colour + 1) % NUM_COLOURS);
}