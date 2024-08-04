#pragma once

#include "bitboard.h"

#include <cstdint>
#include <optional>
#include <vector>

struct SavedMove {
    std::uint32_t source_sq : 6;
    std::uint32_t dest_sq : 6;
    std::uint32_t move_type : 4;
    std::uint32_t prev_castling : 4;
    std::uint32_t prev_quiet_moves : 6;
    // We use A1 to denote no en-passant square as A1 EP is impossible
    std::uint32_t prev_en_passant : 6; 
};

class Board {
private:
    Bitboard bitboard; // 64 
    std::uint16_t total_half_moves {}; // half moves since the start of the game
    std::uint8_t quiet_half_moves {}; // half moves since the last capture/pawn move, max 50
    Colour turn_colour;
    std::uint8_t castling_rights : 4;
    std::optional<Square> en_passant;

    // saved state:
    // prev quiet half moves : needed for all
    // prev en passant : needed for all - could just be a single bit flag with the squares stores separately
    // prev castling : needed for all - could just bit a single bit flag denoting a change with it stored separately
    // captured piece : not needed for all, store separately

    std::vector<SavedMove> prev_moves;
    // 32 pieces on a board, but kings can't be captured.
    std::vector<Piece> captures;
};

void Foo() {
    const auto sz { sizeof(Board) };
    const auto sz2 { sizeof(SavedMove) };
}

/*
 * info needed for a saved move:
 * - source square: 6 bits
 * - dest square: 6 bits
 * - piece: 3 bits (cos 2 pieces might be able to make the same move)
 * - move type: 3 bits
 * - prev castling rights: 4 bits - can be shared
 * - prev half move num 16 bits - can be shared
 * - move type metadata: 6 bits
 * ---- quiet
 * ---- capture
 * -------- captured piece type: 3 bits
 * ---- castle K
 * ---- castle Q
 * ---- en-passant
 * -------- square: 6 bits
 * ---- double pawn push
 * ---- move promotion
 * -------- promoted piece: 3 bits
 * ---- capture promotion
 * -------- captured piece
 * -------- promoted piece: 6 bits
 * 
 * or
 * 
 * quiet
 * capture pawn
 * capture knight
 * capture bishop
 * capture rook
 * capture queen
 * castle K
 * castle Q
 * en-passant
 * double pawn push
 * knight move promotion
 * rook move promotion
 * bishop move promotion
 * queen move promotion
 * knight-knight capture promotion
 * knight-rook capture promotion
 * knight-bishop capture promotion
 * knight-queen capture promotion
 * rook-knight capture promotion
 * rook-rook capture promotion
 * rook-bishop capture promotion
 * rook-queen capture promotion
 * bishop-knight capture promotion
 * bishop-rook capture promotion
 * bishop-bishop capture promotion
 * bishop-queen capture promotion
 * queen-knight capture promotion
 * queen-rook capture promotion
 * queen-bishop capture promotion
 * queen-queen capture promotion : 5 bits
 */