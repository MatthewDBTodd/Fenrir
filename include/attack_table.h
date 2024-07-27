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
    // for kings and knights
    std::uint64_t lookup(const Square square, const Piece piece) const;
    // pawns
    // todo - pawn move type needs improving 
    std::uint64_t lookup(const Colour colour, const Square square, 
                         const PawnMovetype type) const;
    // sliding pieces
    std::uint64_t lookup(const Square square, const Piece piece,
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