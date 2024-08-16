#include "attack_table.h"

#include "assert.h"

std::uint64_t AttackTable::captures(const Square square, const Piece piece, const Colour colour,
                                    const std::uint64_t blockers, const std::uint64_t enemies) const {
    return attacks(square, piece, colour, blockers) & enemies;
}

// Has to have a separate implementation as pawn quiet moves use different move-sets
std::uint64_t AttackTable::moves(const Square square, const Piece piece, const Colour colour,
                                 const std::uint64_t blockers) const {
    BOOST_ASSERT(piece != NUM_PIECES);
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

std::uint64_t AttackTable::attacks(const Square square, const Piece piece, const Colour colour, 
                                   const std::uint64_t blockers) const {
    BOOST_ASSERT(piece != NUM_PIECES);
    switch (piece) {
        case PAWN: return pawn.attacks(colour, square);
        case KNIGHT: return knight[square];
        case BISHOP:
        case ROOK: return sliding_piece.lookup(square, piece, blockers);
        case QUEEN:
            return sliding_piece.lookup(square, BISHOP, blockers) |
                   sliding_piece.lookup(square, ROOK, blockers);
        case KING: return king[square];
        default: return 0ul; // should not happen
    }
}