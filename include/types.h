#pragma once

#include <array>
#include <bit>
#include "fenrir_assert.h"
#include <cstdint>

#ifdef FENRIR_TEST
#include <iostream>
#endif

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

inline constexpr Square from_mask(const std::uint64_t mask) {
    BOOST_ASSERT(std::popcount(mask) == 1);
    if (std::popcount(mask) != 1) {
        __builtin_unreachable();
    }
    return static_cast<Square>(std::countr_zero(mask));
}

inline constexpr std::uint64_t from_square(const Square square) {
    BOOST_ASSERT(square != NUM_SQUARES);
    return (1ul << square);
}

static constexpr std::array<Piece, NUM_PIECES> ALL_PIECES {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

// In descending order of value
static constexpr std::array<Piece, NUM_PIECES-1> CAPTURABLE_PIECES {
    QUEEN, ROOK, BISHOP, KNIGHT, PAWN
};

// In descending order of value
static constexpr std::array<Piece, 4> PROMOTION_PIECES {
    QUEEN, ROOK, BISHOP, KNIGHT
};

// Pawns and kings have special considerations in move gen so are handled separately
// TODO - think of a better name for this
static constexpr std::array<Piece, 4> NORMAL_PIECES {
    KNIGHT, BISHOP, ROOK, QUEEN
};

static constexpr std::array<Piece, 5> NON_KING_PIECES {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN
};

inline constexpr Colour opposite(const Colour colour) {
    return static_cast<Colour>((colour + 1) % NUM_COLOURS);
}

#ifdef FENRIR_TEST

inline std::ostream& operator<<(std::ostream& os, const Colour colour) {
    if (colour == WHITE) {
        os << "White";
    } else if (colour == BLACK) {
        os << "Black";
    } else {
        os << "Unknown colour";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Square square) {
    const char file = 'A' + square % 8;
    const char rank = '1' + square / 8;
    os << file << rank;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Piece piece) {
    switch (piece) {
        case PAWN: os << "Pawn"; break;
        case KNIGHT: os << "Knight"; break;
        case BISHOP: os << "Bishop"; break;
        case ROOK: os << "Rook"; break;
        case QUEEN: os << "Queen"; break;
        case KING: os << "King"; break;
        default: os << "Unknown piece"; break;
    }
    return os;
}

#endif // ifdef FENRIR_TEST