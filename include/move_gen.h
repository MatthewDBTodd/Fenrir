#pragma once

#include "attack_table.h"
#include "bitboard.h"
#include "types.h"

#include <bit>
#include <cassert>
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

struct EncodedMove {
    std::uint16_t source_square : 6;
    std::uint16_t dest_square : 6;
    std::uint16_t move_type : 4;
};

enum class MoveType {
    QUIET, DOUBLE_PAWN_PUSH, CASTLE_KINGSIDE, CASTLE_QUEENSIDE, EN_PASSANT, CAPTURE,
    KNIGHT_PROM, BISHOP_PROM, ROOK_PROM, QUEEN_PROM, KNIGHT_CAP_PROM, BISHOP_CAP_PROM,
    ROOK_CAP_PROM, QUEEN_CAP_PROM,
    NUM_MOVE_TYPES
};

class MoveGen {
public:
    MoveGen(std::vector<EncodedMove> &moves, const Bitboard &bb, const AttackTable &at,
            const Colour friendly_colour, std::optional<Square> en_passant);

    // Made rvalue to prevent mistakes with the object outliving its reference members
    void gen() &&;
private:
    void push_if_legal(const std::uint64_t source, const std::uint64_t dest, const MoveType type);

    void generate_pseudo_pawn_moves();
    void single_pawn_moves(const std::uint64_t single_pawn);
    void single_pawn_captures(const std::uint64_t single_pawn, const std::uint64_t captures, 
                              const Piece capturable, const Colour enemy_colour);
    void single_pawn_quiet_moves(const std::uint64_t single_pawn, std::uint64_t quiet_moves);
    void captures_for_piece_type(const Piece piece_type);
    void captures_for_single_piece(const Piece piece_type, const std::uint64_t single_src_piece);
    void quiet_moves_for_piece_type(const Piece piece_type);

    std::vector<EncodedMove> &moves;
    const Bitboard &bb;
    const AttackTable &at;
    const Colour friendly_colour;
    std::optional<Square> en_passant;

    const std::uint64_t pinned {};
};

std::uint64_t king_attackers(const Bitboard &bb, const AttackTable &at, const Colour colour);
// for a given piece, returns whether it's pinned or not. If it is pinned, it returns 
// a mask of pieces the piece is legally able to move to, if it's not pinned, an empty mask
// std::uint64_t pinned(const std::uint64_t piece_pos, const Bitboard &bb, const AttackTable &at,
//                      const Colour friendly_colour);

// returns a mask of all pinned pieces
std::uint64_t pinned_pieces(const Bitboard &bb, const AttackTable &at, const Colour colour);


/*
struct EncodedMove {
    const std::uint8_t source_square : 6;
    const std::uint8_t dest_square : 6;
    const std::uint8_t piece : 3;
    const std::uint8_t move_type : 3;
    // move metadata
    const std::uint8_t piece1 : 3;
    const std::uint8_t piece2 : 3;
    // const std::uint8_t move_data : 6;
};

// enum class MoveType {
//     QUIET, CAPTURE, DOUBLE_PAWN_PUSH, CASTLE_KINGSIDE, CASTLE_QUEENSIDE, EN_PASSANT, 
//     MOVE_PROMOTION, CAPTURE_PROMOTION
// };

namespace move_type_v {

struct Common {
    Square source;
    Square dest;
    Piece piece;
};

struct Quiet {
    Common common;
};
struct Capture{
    Common common;
    Piece captured_piece;
};

struct DoublePawnPush {
    Common common;
};
struct CastleKingSide {
    Common common;
};
struct CastleQueenSide {
    Common common;
};
struct EnPassant {
    Common common;
};
struct MovePromotion {
    Common common;
    Piece promotion_piece;
};
struct CapturePromotion {
    Common common;
    Piece promotion_piece;
    Piece captured_piece;
};

} // namespace move_type_v;

using DecodedMove = std::variant<
    move_type_v::Quiet,
    move_type_v::Capture,
    move_type_v::DoublePawnPush,
    move_type_v::CastleKingSide,
    move_type_v::CastleQueenSide,
    move_type_v::EnPassant,
    move_type_v::MovePromotion,
    move_type_v::CapturePromotion
>;

DecodedMove decode(const EncodedMove encoded_move);
*/

