#pragma once

#include "pawn.h"
#include "knight.h"
#include "king.h"
#include "sliding_piece.h"

#include <array>
#include <cstdint>

enum PawnMovetype {
    ATTACK,
    MOVE,
};

class AttackTable {
public:
    std::uint64_t captures(const Square square, const Piece piece, const Colour colour,
                           const std::uint64_t blockers, const std::uint64_t enemies) const;

    std::uint64_t moves(const Square square, const Piece piece, const Colour colour,
                        const std::uint64_t blockers) const;

private:
    static constexpr piece::PawnAttackTable pawn {};
    static constexpr std::array<std::uint64_t, NUM_SQUARES> knight {
        piece::generate_knight_att_squares()
    };
    static constexpr std::array<std::uint64_t, NUM_SQUARES> king {
        piece::generate_king_att_squares()
    };
    const SlidingPieceAttacks sliding_piece {};
};