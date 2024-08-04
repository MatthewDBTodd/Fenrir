#include "attack_table.h"

#include <cassert>

std::uint64_t AttackTable::captures(const Square square, const Piece piece, const Colour colour,
                                    const std::uint64_t blockers, const std::uint64_t enemies) const {
    assert(piece != NUM_PIECES);
    switch (piece) {
        case PAWN: return pawn.attacks(colour, square) & enemies;
        case KNIGHT: return knight[square] & enemies;
        case BISHOP:
        case ROOK: return sliding_piece.lookup(square, piece, blockers) & enemies;
        case QUEEN: 
            return (sliding_piece.lookup(square, BISHOP, blockers) |
                    sliding_piece.lookup(square, ROOK, blockers))  & enemies;
        case KING: return king[square] & enemies;
        default: return 0ul; // should not happen
    }
}

std::uint64_t AttackTable::moves(const Square square, const Piece piece, const Colour colour,
                                 const std::uint64_t blockers) const {
    assert(piece != NUM_PIECES);
    switch (piece) {
        case PAWN: return pawn.moves(colour, square) & ~blockers;
        case KNIGHT: return knight[square] & ~blockers;
        case BISHOP:
        case ROOK: return sliding_piece.lookup(square, piece, blockers) & ~blockers;
        case QUEEN: 
            return (sliding_piece.lookup(square, BISHOP, blockers) |
                    sliding_piece.lookup(square, ROOK, blockers)) & ~blockers;
        case KING: return king[square] & ~blockers;
        default: return 0ul; // should not happen
    }
}