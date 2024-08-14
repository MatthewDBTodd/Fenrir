#include "bitboard.h"
#include "masks.h"
#include "move_gen.h"
#include "set_bit_iterator.h"

#include <algorithm>
#include <bit>
#include <cassert>
#include <exception>
#include <utility>

// static MoveType promotion(const Piece promotion_piece) {
//     assert(PAWN < promotion_piece && promotion_piece < KING);
//     switch(promotion_piece) {
//         case KNIGHT : return MoveType::KNIGHT_PROM;
//         case BISHOP : return MoveType::BISHOP_PROM;
//         case ROOK   : return MoveType::ROOK_PROM;
//         case QUEEN  : return MoveType::QUEEN_PROM;
//         default     : return MoveType::NUM_MOVE_TYPES; // should not happen
//     }
// }
// 
// static MoveType capture_promotion(const Piece promotion_piece) {
//     assert(PAWN < promotion_piece && promotion_piece < KING);
//     switch(promotion_piece) {
//         case KNIGHT : return MoveType::KNIGHT_CAP_PROM;
//         case BISHOP : return MoveType::BISHOP_CAP_PROM;
//         case ROOK   : return MoveType::ROOK_CAP_PROM;
//         case QUEEN  : return MoveType::QUEEN_CAP_PROM;
//         default     : return MoveType::NUM_MOVE_TYPES; // should not happen
//     }
// }

static bool is_promotion(const std::uint64_t targets) {
    static constexpr std::uint64_t PROMOTION_RANKS {
        (1ul << A1) | (1ul << B1) | (1ul << C1) | (1ul << D1) |
        (1ul << E1) | (1ul << F1) | (1ul << G1) | (1ul << H1) |
        (1ul << A8) | (1ul << B8) | (1ul << C8) | (1ul << D8) |
        (1ul << E8) | (1ul << F8) | (1ul << G8) | (1ul << H8)
    };

    return (targets & PROMOTION_RANKS) > 0;
}

MoveGen::MoveGen(std::vector<EncodedMove> &moves, 
                 const Bitboard &bb, 
                 const AttackTable &at,
                 const Colour friendly_colour, 
                 CastlingRights castling, 
                 std::optional<Square> en_passant) :
        MoveGen(moves, bb, at, friendly_colour, castling, en_passant, 
                king_danger_squares(bb, at, friendly_colour))
{}

MoveGen::MoveGen(std::vector<EncodedMove> &moves, 
                 const Bitboard &bb, 
                 const AttackTable &at,
                 const Colour friendly_colour, 
                 CastlingRights castling, 
                 std::optional<Square> en_passant,
                 const KingInfo king_info) :
        moves(moves),
        bb(bb),
        at(at),
        friendly_colour(friendly_colour),
        castling_rights(castling),
        en_passant(en_passant),
        pinned(pinned_pieces(bb, at, friendly_colour)),
        danger_squares(king_info.king_danger_squares),
        checking_pieces(king_info.king_checking_pieces),
        check_intervention_squares(king_info.check_intervention_squares)
{}

// TODO - castling, checks
void MoveGen::gen() && {
    // If in check by more than 1 piece, the only way to get out of it is to move
    // the king
    if (std::popcount(checking_pieces) > 1) {
        return king_moves();

    /* if there's only a single checker, the options are: 
     * 1. Capture the checking piece
     * 2. A non-king piece blocking the check (if the checker is a sliding piece)
     * 3. Move the king */
    } else if (std::popcount(checking_pieces) == 1) {
        
    // Not in check
    } else {

    }

    castling(KING);
    castling(QUEEN);

    generate_pseudo_pawn_moves();

    // TODO - remove king from NON_PAWN_PIECES as they have special functions
    for (const auto piece_type : NON_PAWN_PIECES) {
        captures_for_piece_type(piece_type);
    }


    // TODO - remove king from NON_PAWN_PIECES as they have special functions
    for (const auto piece_type : NON_PAWN_PIECES) {
        quiet_moves_for_piece_type(piece_type);
    }
}

static bool in_line_with_king(const std::uint64_t source, const std::uint64_t dest,
                              const std::uint64_t king_pos) {
    return direction::SOURCE_DEST_MASKS[from_mask(king_pos)][from_mask(source)] & dest;
}

// TODO - handle en-passant legal check
// TODO - check king move legal check
void MoveGen::push_if_legal(
    const MoveType type, const std::uint64_t source, const std::uint64_t dest,
    const Piece piece, const Piece captured_piece, const Piece promoted_piece
) {
    if (!(source & pinned) || 
        in_line_with_king(source, dest, bb.colour_piece_mask(friendly_colour, KING))
    ) {
        moves.emplace_back(static_cast<std::uint32_t>(type),
                           static_cast<std::uint32_t>(from_mask(source)),
                           static_cast<std::uint32_t>(from_mask(dest)),
                           static_cast<std::uint32_t>(piece),
                           static_cast<std::uint32_t>(captured_piece),
                           static_cast<std::uint32_t>(promoted_piece));
    }
}

void MoveGen::king_moves() {
    const Square king_sq { from_mask(bb.colour_piece_mask(friendly_colour, KING)) };
    const std::uint64_t blockers { bb.entire_mask() };
    std::uint64_t king_attacks { 
        at.attacks(king_sq, KING, friendly_colour, blockers) 
    };
    king_attacks ^= danger_squares;
    for (const auto capturable : CAPTURABLE_PIECES) {
        const std::uint64_t captures_of_piece {
            bb.colour_piece_mask(opposite(friendly_colour), capturable) & king_attacks
        };
        king_attacks ^= captures_of_piece;
        for (const auto capture_of_piece : SetBits(captures_of_piece)) {
            // don't need to do legal check as we've already covered king danger
            // squares, and the other legal checks (pins/en-passant) don't apply 
            // to the king
            moves.emplace_back(static_cast<std::uint32_t>(MoveType::CAPTURE),
                               static_cast<std::uint32_t>(king_sq),
                               static_cast<std::uint32_t>(from_mask(capture_of_piece)),
                               static_cast<std::uint32_t>(KING),
                               static_cast<std::uint32_t>(capturable),
                               static_cast<std::uint32_t>(NUM_PIECES));
        }
    }
    // all remaining moves are quiet moves
    for (const auto quiet_move : SetBits(king_attacks)) {
        moves.emplace_back(static_cast<std::uint32_t>(MoveType::QUIET),
                           static_cast<std::uint32_t>(king_sq),
                           static_cast<std::uint32_t>(from_mask(quiet_move)),
                           static_cast<std::uint32_t>(KING),
                           static_cast<std::uint32_t>(NUM_PIECES),
                           static_cast<std::uint32_t>(NUM_PIECES));
    }
}

// We invalidate castling if the king/rook ever moves, so all we need to check is
// if the path is clear and whether the intermediate squares are under attack.
// This means things fall over if the castling rights we pass in are incorrect, we
// assume if castling is allowed the king/rook are on their original squares.
void MoveGen::castling(const Piece side) {
    assert(side == KING || side == QUEEN);

    if (!castling_rights.can_castle(friendly_colour, side)) {
        return;
    }

    const std::uint64_t castling_squares { [=, this] {
        if (friendly_colour == WHITE) {
            return side == KING ? from_square(F1) | from_square(G1)
                                : from_square(C1) | from_square(D1);
        } else {
            return side == KING ? from_square(F8) | from_square(G8)
                                : from_square(C8) | from_square(D8);
        }
    }() };

    const Square king_sq { friendly_colour == WHITE ? E1 : E8 };
    const Square dest_sq { [=, this] {
        if (friendly_colour == WHITE) {
            return side == KING ? G1 : C1;
        } else {
            return side == KING ? G8 : C8;
        }
    }() };
#ifndef NDEBUG
    const Square rook_sq { [=, this] {
        if (friendly_colour == WHITE) {
            return side == KING ? H1 : A1;
        } else {
            return side == KING ? H8 : A8;
        }
    }() };
#endif

    // check the king hasn't moved
    assert(from_mask(bb.colour_piece_mask(friendly_colour, KING)) == king_sq);
    // check the rook hasn't moved
    assert(bb.colour_piece_mask(friendly_colour, ROOK) & from_square(rook_sq));

    //     are the intermediate squares blocked? | are the intermediate squares under attack?
    if ( !(castling_squares & bb.entire_mask() || castling_squares & danger_squares) ) {
        const auto type { side == KING ? MoveType::CASTLE_KINGSIDE : MoveType::CASTLE_QUEENSIDE };
        moves.emplace_back(static_cast<std::uint32_t>(type),
                            static_cast<std::uint32_t>(king_sq),
                            static_cast<std::uint32_t>(dest_sq),
                            static_cast<std::uint32_t>(KING),
                            static_cast<std::uint32_t>(NUM_PIECES),
                            static_cast<std::uint32_t>(NUM_PIECES));
    }
}

void MoveGen::quiet_moves_for_piece_type(const Piece piece_type) {
    const std::uint64_t all_pieces { bb.entire_mask() };
    const std::uint64_t all_src_pieces { bb.colour_piece_mask(friendly_colour, piece_type) };
    for (const std::uint64_t single_src_piece : SetBits(all_src_pieces)) {
        const std::uint64_t quiet_moves { 
            at.moves(from_mask(single_src_piece), piece_type, friendly_colour, all_pieces)
        };
        for (const auto single_move : SetBits(quiet_moves)) {
            push_if_legal(MoveType::QUIET, single_src_piece, single_move, piece_type,
                          NUM_PIECES, NUM_PIECES);
        }
    }
}

void MoveGen::captures_for_piece_type(const Piece piece_type) {
    const std::uint64_t all_src_pieces { bb.colour_piece_mask(friendly_colour, piece_type) };
    for (const std::uint64_t single_src_piece : SetBits(all_src_pieces)) {
        captures_for_single_piece(piece_type, single_src_piece);
    }
}

void MoveGen::captures_for_single_piece(
    const Piece piece_type, const std::uint64_t single_src_piece
) {
    const Colour enemy_colour { opposite(friendly_colour) }; 
    const std::uint64_t enemy_pieces_mask { bb.colour_mask(enemy_colour) };
    const std::uint64_t all_pieces { bb.entire_mask() };

    const std::uint64_t captures { 
        at.captures(from_mask(single_src_piece), piece_type, friendly_colour, 
                                all_pieces, enemy_pieces_mask) 
    };

    if (captures == 0) {
        return;
    }

    for (const Piece capturable_piece : CAPTURABLE_PIECES) {
        const std::uint64_t captures_of_piece {
            captures & bb.colour_piece_mask(enemy_colour, capturable_piece)
        };
        for (const auto single_capture : SetBits(captures_of_piece)) {
            push_if_legal(MoveType::CAPTURE, single_src_piece, single_capture,
                          piece_type, capturable_piece, NUM_PIECES);
        }
    }
}

void MoveGen::single_pawn_quiet_moves(
    const std::uint64_t single_pawn, std::uint64_t quiet_moves
) {
    assert(std::popcount(quiet_moves) <= 2);
    // single + double pawn push
    if (std::popcount(quiet_moves) == 2) {
        const auto first { quiet_moves & -quiet_moves };
        quiet_moves ^= first;
        // least significant bit is single push for white, double for black
        const MoveType first_type = friendly_colour == WHITE ? MoveType::QUIET 
                                                             : MoveType::DOUBLE_PAWN_PUSH;
        push_if_legal(first_type, single_pawn, first, PAWN, NUM_PIECES, NUM_PIECES);

        const MoveType second_type = friendly_colour == WHITE ? MoveType::DOUBLE_PAWN_PUSH
                                                                : MoveType::QUIET;
        push_if_legal(second_type, single_pawn, quiet_moves, PAWN, NUM_PIECES, NUM_PIECES);
    } else if (is_promotion(quiet_moves)) {
        for (const auto promotion_piece : PROMOTION_PIECES) {
            push_if_legal(MoveType::MOVE_PROMOTION, single_pawn, quiet_moves, PAWN,
                          NUM_PIECES, promotion_piece);
        }
    } else {
        push_if_legal(MoveType::QUIET, single_pawn, quiet_moves, PAWN, NUM_PIECES, NUM_PIECES);
    }
}

void MoveGen::single_pawn_captures(const std::uint64_t single_pawn, const std::uint64_t captures,
                                   const Piece capturable, const Colour enemy_colour) {
    const auto captures_of_piece { captures & bb.colour_piece_mask(
        enemy_colour, capturable
    ) };
    for (const auto single_capture : SetBits(captures_of_piece)) {
        if (is_promotion(captures)) {
            for (const auto promotion_piece : PROMOTION_PIECES) {
                push_if_legal(MoveType::CAPTURE_PROMOTION, single_pawn, single_capture,
                              PAWN, capturable, promotion_piece);
            }
        } else {
            push_if_legal(MoveType::CAPTURE, single_pawn, single_capture, PAWN,
                          capturable, NUM_PIECES);
        } 
    }
}

void MoveGen::single_pawn_moves(const std::uint64_t single_pawn) {
    const Colour enemy_colour { opposite(friendly_colour) };
    const std::uint64_t ep_mask { en_passant ? from_square(*en_passant) : 0ul };
    const std::uint64_t enemy_pieces_mask { bb.colour_mask(enemy_colour) | ep_mask };
    const std::uint64_t captures { 
        at.captures(from_mask(single_pawn), PAWN, friendly_colour, 0ul, enemy_pieces_mask) };
    assert(std::popcount(captures) <= 2);

    if (captures > 0) {
        for (const auto capturable : CAPTURABLE_PIECES) {
            single_pawn_captures(single_pawn, captures, capturable, enemy_colour);
        } 
    }

    const std::uint64_t ep_captures { ep_mask & captures };
    if (ep_captures > 0) {
        assert(std::popcount(ep_captures) == 1);
        push_if_legal(MoveType::EN_PASSANT, single_pawn, ep_mask, PAWN, PAWN, NUM_PIECES);
    }

    const std::uint64_t all_pieces { bb.entire_mask() };
    const std::uint64_t quiet_moves {
        at.moves(from_mask(single_pawn), PAWN, friendly_colour, all_pieces)
    };

    if (quiet_moves > 0) {
        single_pawn_quiet_moves(single_pawn, quiet_moves);
    }
}

void MoveGen::generate_pseudo_pawn_moves() {
    const std::uint64_t all_pawns { bb.colour_piece_mask(friendly_colour, PAWN) };
    for (const auto single_pawn : SetBits(all_pawns)) {
        single_pawn_moves(single_pawn);
    }
}

// // TODO - can combine king_attackers and king_danger_squares in a single sweep for efficiency
// std::uint64_t king_attackers(const Bitboard &bb, const AttackTable &at, const Colour colour) {
//     const std::uint64_t king_pos { bb.colour_piece_mask(colour, KING) };
//     const Square king_sq { from_mask(king_pos) };
//     const Colour enemy_colour { opposite(colour) };
//     const std::uint64_t occupied { bb.entire_mask() };
//     const std::uint64_t enemies { bb.colour_mask(enemy_colour) };
//     std::uint64_t king_attackers {};
//     for (const Piece piece : ALL_PIECES) {
//         const std::uint64_t piece_mask { bb.colour_piece_mask(enemy_colour, piece) };
//         king_attackers |= (piece_mask & at.captures(king_sq, piece, colour, occupied, enemies));
//     }
//     return king_attackers;
// }

KingInfo king_danger_squares(const Bitboard &bb, const AttackTable &at, const Colour colour) {
    std::uint64_t king_danger_squares {};
    std::uint64_t king_checking_pieces {};
    std::uint64_t check_intervention_squares {};

    const auto king_pos { bb.colour_piece_mask(colour, KING) };
    // remove the king as a king will still be in danger if moving backwards along the
    // ray of a sliding piece
    const std::uint64_t blockers { bb.entire_mask() ^ king_pos };

    for (const Piece piece_type : ALL_PIECES) {
        const std::uint64_t pieces { bb.colour_piece_mask(opposite(colour), piece_type) };
        for (const auto piece : SetBits(pieces)) {
            const auto attacks { at.attacks(from_mask(piece), piece_type, colour, blockers) };
            king_danger_squares |= attacks;
            if (!(attacks & king_pos)) { 
                continue;
            }
            // check!
            king_checking_pieces |= piece;
            // intervention by capturing the checking piece
            check_intervention_squares |= piece;
            /* intervention by blocking the check
                * 1. If the checking piece is a pawn, this is a no-op: (source --> dest) straight
                *    lines for two adjacent pieces do not intersect:
                *
                *    8 - - - - - - - -    - X - - - - - -    - - - - - - - -
                *    7 - - P - - - - -    - - X - - - - -    - - - - - - - -
                *    6 - - - X - - - -    - - - K - - - -    - - - - - - - -
                *    5 - - - - X - - -    - - - - - - - -    - - - - - - - -
                *    4 - - - - - X - -  & - - - - - - - -  = - - - - - - - - 
                *    3 - - - - - - X -    - - - - - - - -    - - - - - - - -
                *    2 - - - - - - - X    - - - - - - - -    - - - - - - - -
                *    1 - - - - - - - -    - - - - - - - -    - - - - - - - -
                *      A B C D E F G H    A B C D E F G H    A B C D E F G H
                * 
                *    Pawn is on C7, King on D6, masks are marked with X, you can see they do not
                *    intersect.
                * 2. If the checking piece is a knight, this is a no-op: (source --> dest) is not a 
                *    straight line so the source_dest_mask will be 0
                * 3. If the checking piece is a sliding piece, then the attack mask will go past
                *    the king position, as we removed the king from the board mask so we could
                *    accurately calculate danger squares for the king. So we AND the two
                *    (source --> dest) straight line masks to get the squares in-between the king
                *    and the checking piece:
                *    
                *    8 - - - X - - - -    - - - - - - - -    - - - - - - - -
                *    7 - - - X - - - -    - - - R - - - -    - - - R - - - -
                *    6 - - - X - - - -    - - - X - - - -    - - - X - - - -
                *    5 - - - X - - - -    - - - X - - - -    - - - X - - - -
                *    4 - - - X - - - -  & - - - X - - - -  = - - - X - - - -
                *    3 - - - K - - - -    - - - X - - - -    - - - K - - - -
                *    2 - - - - - - - -    - - - X - - - -    - - - - - - - -
                *    1 - - - - - - - -    - - - X - - - -    - - - - - - - -
                *      A B C D E F G H    A B C D E F G H    A B C D E F G H
                * 
                *  */
            check_intervention_squares |= (
                direction::SOURCE_DEST_MASKS[from_mask(king_pos)][from_mask(piece)] &
                direction::SOURCE_DEST_MASKS[from_mask(piece)][from_mask(king_pos)]
            );
        }
    }
    return KingInfo {
        king_danger_squares,
        king_checking_pieces,
        check_intervention_squares
    };
}

std::uint64_t pinned_pieces(const Bitboard &bb, const AttackTable &at, const Colour colour) {
    const std::uint64_t king_pos { bb.colour_piece_mask(colour, KING) };
    const Square king_sq { from_mask(king_pos) };
    const std::uint64_t enemy_queens { bb.colour_piece_mask(opposite(colour), QUEEN) };
    // pretend the queen is a rook/bishop for the sake of pin calculations
    const std::uint64_t enemy_rooks { 
        bb.colour_piece_mask(opposite(colour), ROOK) | enemy_queens
    };
    const std::uint64_t enemy_bishops { 
        bb.colour_piece_mask(opposite(colour), BISHOP) | enemy_queens
    };

    // place a rook/bishop in the friendly king position and see which friendly pieces it would
    // attack, which gives us candidates for pinned pieces
    const std::uint64_t rook_pin_candidates {
        at.captures(king_sq, ROOK, opposite(colour), bb.entire_mask(), bb.colour_mask(colour))
    };
    const std::uint64_t bishop_pin_candidates {
        at.captures(king_sq, BISHOP, opposite(colour), bb.entire_mask(), bb.colour_mask(colour))
    };

    std::uint64_t rv {};

    for (const auto rook_pin_candidate : SetBits(rook_pin_candidates)) {
        // get rooks in line with candidate if any
        const std::uint64_t rooks_in_line { 
            direction::SOURCE_DEST_MASKS[king_sq][from_mask(rook_pin_candidate)] & enemy_rooks 
        };
        for (const auto rook_in_line : SetBits(rooks_in_line)) {
            // cppcheck-suppress useStlAlgorithm
            rv |= (
                at.captures(from_mask(rook_in_line), 
                            ROOK, 
                            opposite(colour), 
                            bb.entire_mask(), 
                            bb.colour_mask(colour)) & rook_pin_candidate
            );
        };
    }

    for (const auto bishop_pin_candidate : SetBits(bishop_pin_candidates)) {
        // get bishops in line with candidate if any
        const std::uint64_t bishops_in_line { 
            direction::SOURCE_DEST_MASKS[king_sq][from_mask(bishop_pin_candidate)] & enemy_bishops 
        };
        for (const auto bishop_in_line : SetBits(bishops_in_line)) {
            // cppcheck-suppress useStlAlgorithm
            rv |= (
                at.captures(from_mask(bishop_in_line), 
                            BISHOP, 
                            opposite(colour), 
                            bb.entire_mask(), 
                            bb.colour_mask(colour)) & bishop_pin_candidate
            );
        };
    }

    return rv;
}

static Square ep_square(const Square dest_square) {
    static constexpr std::uint64_t white_double_push_squares {
        (1ul << A4) | (1ul << B4) | (1ul << C4) | (1ul << D4) |
        (1ul << E4) | (1ul << F4) | (1ul << G4) | (1ul << H4)
    };

    static constexpr std::uint64_t black_double_push_squares {
        (1ul << A5) | (1ul << B5) | (1ul << C5) | (1ul << D5) |
        (1ul << E5) | (1ul << F5) | (1ul << G5) | (1ul << H5)
    };
    const std::uint64_t square_mask { from_square(dest_square) };

    assert((square_mask & white_double_push_squares) || 
           (square_mask & black_double_push_squares));
    
    (void)black_double_push_squares;

    if (square_mask & white_double_push_squares) {
        return from_mask(direction::south(square_mask));
    } else {
        return from_mask(direction::north(square_mask));
    }
}

static Square ep_pawn_square(const Square dest_square) {
    static constexpr std::uint64_t white_double_push_squares {
        (1ul << A4) | (1ul << B4) | (1ul << C4) | (1ul << D4) |
        (1ul << E4) | (1ul << F4) | (1ul << G4) | (1ul << H4)
    };

    static constexpr std::uint64_t black_double_push_squares {
        (1ul << A5) | (1ul << B5) | (1ul << C5) | (1ul << D5) |
        (1ul << E5) | (1ul << F5) | (1ul << G5) | (1ul << H5)
    };
    const std::uint64_t square_mask { from_square(dest_square) };

    assert((square_mask & white_double_push_squares) || 
           (square_mask & black_double_push_squares));
    
    (void)black_double_push_squares;

    if (square_mask & white_double_push_squares) {
        return from_mask(direction::north(square_mask));
    } else {
        return from_mask(direction::south(square_mask));
    }
}

DecodedMove decode(const EncodedMove encoded_move) {
    move_type_v::Common common {
        static_cast<Square>(encoded_move.source_square),
        static_cast<Square>(encoded_move.dest_square),
        static_cast<Piece>(encoded_move.piece),
        static_cast<Colour>(encoded_move.colour)
    };
    MoveType move_type { static_cast<MoveType>(encoded_move.move_type) };
    switch (move_type) {
        case MoveType::QUIET : 
            return move_type_v::Quiet { common };
        case MoveType::CAPTURE : {
            assert(encoded_move.captured_piece < NUM_PIECES);
            const Piece captured { static_cast<Piece>(encoded_move.captured_piece) }; 
            return move_type_v::Capture { common, captured };}
        case MoveType::DOUBLE_PAWN_PUSH : 
            return move_type_v::DoublePawnPush { common, ep_square(common.dest) };
        case MoveType::CASTLE_KINGSIDE : 
            return move_type_v::CastleKingSide { common };
        case MoveType::CASTLE_QUEENSIDE : 
            return move_type_v::CastleQueenSide { common };
        case MoveType::EN_PASSANT : 
            return move_type_v::EnPassant { common, ep_pawn_square(common.dest) };
        case MoveType::MOVE_PROMOTION : {
            assert(encoded_move.promoted_piece < NUM_PIECES);
            const Piece promoted { static_cast<Piece>(encoded_move.promoted_piece) };
            return move_type_v::MovePromotion { common, promoted };}
        case MoveType::CAPTURE_PROMOTION : {
            assert(encoded_move.captured_piece < NUM_PIECES);
            assert(encoded_move.promoted_piece < NUM_PIECES);
            const Piece promoted { static_cast<Piece>(encoded_move.promoted_piece) };
            const Piece captured { static_cast<Piece>(encoded_move.captured_piece) };
            return move_type_v::CapturePromotion { common, captured, promoted };}
        default : 
            throw std::invalid_argument("Unrecognised move type"); // should never happen
    }
}