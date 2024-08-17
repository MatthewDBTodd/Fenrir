#pragma once

#include "attack_table.h"
#include "castling.h"
#include "encoded_move.h"
#include "move_types.h"
#include "types.h"

#include <bit>
#include "assert.h"
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

#ifndef NDEBUG
#include <iostream>
#endif

class Bitboard;

// returns a mask of all pinned pieces
std::uint64_t pinned_pieces(const Bitboard &bb, const AttackTable &at, const Colour colour);

struct KingInfo {
    std::uint64_t king_danger_squares; // all squares under attack
    std::uint64_t king_checking_pieces; 
    // Squares friendly pieces can move to to block check or capture the checking piece.
    // If the number of checking pieces > 1 then this value is ignored for non-king pieces.
    // If the checking piece is a non-sliding piece, then 
    // king_check_blocking_squares will == king_checking_pieces, as pieces can only
    // intervene by capturing that piece
    std::uint64_t check_intervention_squares;
};

KingInfo king_danger_squares(const Bitboard &bb, const AttackTable &at, const Colour colour);

// While the above function calculates all checking pieces, along with danger/intervention squares
// sometimes we just want to know as efficiently as possible whether the king in check, e.g. for 
// legality of en-passant moves
bool king_in_check(const Bitboard &bb, const AttackTable &at, const Colour colour);

class MoveGen {
public:
    MoveGen(std::vector<EncodedMove> &moves, Bitboard &bb, const AttackTable &at,
            const Colour friendly_colour, CastlingRights castling, std::optional<Square> en_passant);

    // Made rvalue to prevent mistakes with the object outliving its reference members
    void gen() &&;
private:
    MoveGen(std::vector<EncodedMove> &moves, Bitboard &bb, const AttackTable &at,
            const Colour friendly_colour, CastlingRights castling, std::optional<Square> en_passant,
            const KingInfo king_info);

    void push_if_legal(const MoveType type, const std::uint64_t source, const std::uint64_t dest, 
                       const Piece piece, const Piece captured_piece, const Piece promoted_piece);

    void escape_single_check();
    void generate_pawn_moves();
    void single_pawn_moves(const std::uint64_t single_pawn);
    void single_pawn_captures(const std::uint64_t single_pawn, const std::uint64_t captures, 
                              const Piece capturable, const Colour enemy_colour);
    void single_pawn_quiet_moves(const std::uint64_t single_pawn, std::uint64_t quiet_moves);
    void captures_for_piece_type(const Piece piece_type);
    void captures_for_single_piece(const Piece piece_type, const std::uint64_t single_src_piece);
    void quiet_moves_for_piece_type(const Piece piece_type);

    void king_moves();
    void castling(const Piece side);

    std::vector<EncodedMove> &moves;
    Bitboard &bb;
    const AttackTable &at;
    const Colour friendly_colour;
    CastlingRights castling_rights;
    std::optional<Square> en_passant;

    const std::uint64_t pinned {};
    const std::uint64_t danger_squares {};
    const std::uint64_t checking_pieces {};
    const std::uint64_t check_intervention_squares {};
};

#ifndef NDEBUG

std::ostream& operator<<(std::ostream& os, const EncodedMove move);

#endif