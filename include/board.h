#pragma once

#include "bitboard.h"

#include <cstdint>
#include <optional>

class Board {
private:
    Bitboard bitboard; // 64 
    std::uint16_t total_half_moves {}; // half moves since the start of the game
    std::uint16_t quiet_half_moves {}; // half moves since the last capture/pawn move
    Colour turn_colour;
    std::uint8_t castling_rights : 4;
    std::optional<Square> en_passant;
};

/*
 * info needed for a saved move:
 * - source square: 6 bits
 * - dest square: 6 bits
 * - piece: 3 bits (cos 2 pieces might be able to make the same move)
 * - move type: 3 bits
 * - prev castling rights: 4 bits
 * - prev half move num 16 bits
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
 */