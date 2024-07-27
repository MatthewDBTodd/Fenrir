#include "move_gen.h"

std::uint64_t king_attackers(const Bitboard &bb, const AttackTable &at, const Colour colour) {
    const std::uint64_t king_pos { bb.colour_piece_mask(colour, KING) };
    const Square king_sq { from_mask(king_pos) };
    const Colour enemy_colour { opposite(colour) };
    const std::uint64_t occupied { bb.entire_mask() };
    std::uint64_t king_attackers {};
    for (const Piece piece : ALL_PIECES) {
        const std::uint64_t piece_mask { bb.colour_piece_mask(enemy_colour, piece) };
        const std::uint64_t attacks = [=, &at] {
            switch (piece) {
                case PAWN: return piece_mask & at.lookup(colour, king_sq, ATTACK);
                case KNIGHT:
                case KING: return piece_mask & at.lookup(king_sq, piece);
                case BISHOP:
                case ROOK:
                case QUEEN: return piece_mask & at.lookup(king_sq, piece, occupied);
            }
        }();
        king_attackers |= attacks;
    }
    return king_attackers;
}
