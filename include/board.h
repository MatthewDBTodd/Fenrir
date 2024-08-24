#pragma once

#include "bitboard.h"
#include "castling.h"
#include "decoded_move.h"

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

struct SavedMove {
    const DecodedMove move;
    // from before the move was made
    const CastlingRights prev_castling;
    const std::uint16_t prev_quiet_half_moves;
    const std::optional<Square> prev_en_passant;
};

class Board {
public:
    static std::optional<Board> from_fen(std::string_view fen);

    void make_move(const DecodedMove &move);
    void undo_last_move();

    void operator()(const move_type_v::Quiet &quiet);
    void operator()(const move_type_v::Capture &cap);
    void operator()(const move_type_v::DoublePawnPush &dpp);
    void operator()(const move_type_v::CastleKingSide &cks);
    void operator()(const move_type_v::CastleQueenSide &cqs);
    void operator()(const move_type_v::EnPassant &ep);
    void operator()(const move_type_v::MovePromotion &mp);
    void operator()(const move_type_v::CapturePromotion &cp);

    Bitboard& bitboard() { return bitboard_; }
    Colour turn_colour() const { return turn_colour_; } 
    CastlingRights castling_rights() const { return castling_; }
    std::optional<Square> en_passant() const { return en_passant_; }

#ifndef FENRIR_TEST
private:
#endif

    Board(const Bitboard &bb, const std::uint16_t fullmove_count, 
          const std::uint16_t quiet_half_moves, const Colour turn_colour,
          const CastlingRights castling, const std::optional<Square> en_passant);

    Bitboard bitboard_; // 64 
    // Starts at 1 and increments after blacks move. Apparently the most moves in a game
    // of chess ever was 269 so best not to risk using a uint8_t
    std::uint16_t fullmove_count_ {}; 
    std::uint8_t quiet_half_moves_ {}; // half moves since the last capture/pawn move, max 50
    Colour turn_colour_ { WHITE };
    CastlingRights castling_ {};
    std::optional<Square> en_passant_ {};
    std::vector<SavedMove> prev_moves_;
};

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