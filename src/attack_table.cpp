#include "attack_table.h"

#include <cassert>

std::uint64_t AttackTable::lookup(const Square square, const Piece piece) const {
    assert(piece == KING || piece == KNIGHT);
    if (piece == KING) {
        return king[square];
    } else {
        return knight[square];
    }
}

std::uint64_t AttackTable::lookup(const Colour colour, const Square square,
                                  const PawnMovetype type) const {

    return pawn.lookup(colour, static_cast<piece::PawnAttackTable::Type>(type), square);
}

std::uint64_t AttackTable::lookup(const Square square, const Piece piece, 
                                  const std::uint64_t blockers) const {
    return sliding_piece.lookup(square, piece, blockers);
}