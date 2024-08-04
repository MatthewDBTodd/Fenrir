#include "move_gen.h"
#include "set_bit_iterator.h"

#include <algorithm>
#include <bit>
#include <exception>
#include <utility>

static MoveType promotion(const Piece promotion_piece) {
    assert(PAWN < promotion_piece && promotion_piece < KING);
    switch(promotion_piece) {
        case KNIGHT : return MoveType::KNIGHT_PROM;
        case BISHOP : return MoveType::BISHOP_PROM;
        case ROOK   : return MoveType::ROOK_PROM;
        case QUEEN  : return MoveType::QUEEN_PROM;
        default     : return MoveType::NUM_MOVE_TYPES; // should not happen
    }
}

static MoveType capture_promotion(const Piece promotion_piece) {
    assert(PAWN < promotion_piece && promotion_piece < KING);
    switch(promotion_piece) {
        case KNIGHT : return MoveType::KNIGHT_CAP_PROM;
        case BISHOP : return MoveType::BISHOP_CAP_PROM;
        case ROOK   : return MoveType::ROOK_CAP_PROM;
        case QUEEN  : return MoveType::QUEEN_CAP_PROM;
        default     : return MoveType::NUM_MOVE_TYPES; // should not happen
    }
}

static bool is_promotion(const std::uint64_t targets) {
    static constexpr std::uint64_t PROMOTION_RANKS {
        (1ull << A1) | (1ull << B1) | (1ull << C1) | (1ull << D1) |
        (1ull << E1) | (1ull << F1) | (1ull << G1) | (1ull << H1) |
        (1ull << A8) | (1ull << B8) | (1ull << C8) | (1ull << D8) |
        (1ull << E8) | (1ull << F8) | (1ull << G8) | (1ull << H8)
    };

    return (targets & PROMOTION_RANKS) > 0;
}

MoveGen::MoveGen(std::vector<EncodedMove> &moves, const Bitboard &bb, const AttackTable &at,
                 const Colour friendly_colour, std::optional<Square> en_passant) :
        moves(moves),
        bb(bb),
        at(at),
        friendly_colour(friendly_colour),
        en_passant(en_passant)
{}

void MoveGen::gen() && {
    generate_pseudo_pawn_moves();

    for (const auto piece_type : NON_PAWN_PIECES) {
        captures_for_piece_type(piece_type);
    }


    for (const auto piece_type : NON_PAWN_PIECES) {
        quiet_moves_for_piece_type(piece_type);
    }
}

void MoveGen::quiet_moves_for_piece_type(const Piece piece_type) {
    const std::uint64_t all_pieces { bb.entire_mask() };
    const std::uint64_t all_src_pieces { bb.colour_piece_mask(friendly_colour, piece_type) };
    for (const std::uint64_t single_src_piece : SetBits(all_src_pieces)) {
        const std::uint64_t quiet_moves { 
            at.moves(from_mask(single_src_piece), piece_type, friendly_colour, all_pieces)
        };
        std::transform(SetBits(quiet_moves).begin(), SetBits(quiet_moves).end(),
                       std::back_inserter(moves), [=](const auto single_move) {
            return EncodedMove {
                static_cast<std::uint16_t>(from_mask(single_src_piece)),
                static_cast<std::uint16_t>(from_mask(single_move)),
                static_cast<std::uint16_t>(MoveType::QUIET) };
        });
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
        std::transform(SetBits(captures_of_piece).begin(), SetBits(captures_of_piece).end(),
                       std::back_inserter(moves), [=](const auto single_capture) {
            return EncodedMove {
                static_cast<std::uint16_t>(from_mask(single_src_piece)),
                static_cast<std::uint16_t>(from_mask(single_capture)),
                static_cast<std::uint16_t>(MoveType::CAPTURE) };
        });
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
        moves.emplace_back(
            static_cast<std::uint16_t>(from_mask(single_pawn)),
            static_cast<std::uint16_t>(from_mask(first)),
            static_cast<std::uint16_t>(first_type)
        );
        const MoveType second_type = friendly_colour == WHITE ? MoveType::DOUBLE_PAWN_PUSH
                                                                : MoveType::QUIET;
        moves.emplace_back(
            static_cast<std::uint16_t>(from_mask(single_pawn)),
            static_cast<std::uint16_t>(from_mask(quiet_moves)),
            static_cast<std::uint16_t>(second_type)
        );
    } else if (is_promotion(quiet_moves)) {
        std::transform(PROMOTION_PIECES.begin(), PROMOTION_PIECES.end(), 
                       std::back_inserter(moves), [=](const auto promotion_piece) {
            return EncodedMove {
                static_cast<std::uint16_t>(from_mask(single_pawn)),
                static_cast<std::uint16_t>(from_mask(quiet_moves)),
                static_cast<std::uint16_t>(promotion(promotion_piece)) };
        });
    } else {
        moves.emplace_back(
            static_cast<std::uint16_t>(from_mask(single_pawn)),
            static_cast<std::uint16_t>(from_mask(quiet_moves)),
            static_cast<std::uint16_t>(MoveType::QUIET)
        );
    }
}

void MoveGen::single_pawn_captures(const std::uint64_t single_pawn, const std::uint64_t captures,
                                   const Piece capturable, const Colour enemy_colour) {
    const auto captures_of_piece { captures & bb.colour_piece_mask(
        enemy_colour, capturable
    ) };
    for (const auto single_capture : SetBits(captures_of_piece)) {
        if (is_promotion(captures)) {
            std::transform(PROMOTION_PIECES.begin(), PROMOTION_PIECES.end(),
                           std::back_inserter(moves), [=](const auto promotion_piece) {
                return EncodedMove {
                    static_cast<std::uint16_t>(from_mask(single_pawn)),
                    static_cast<std::uint16_t>(from_mask(single_capture)),
                    static_cast<std::uint16_t>(capture_promotion(promotion_piece)) };
            });
        } else {
            moves.emplace_back(
                static_cast<std::uint16_t>(from_mask(single_pawn)),
                static_cast<std::uint16_t>(from_mask(single_capture)),
                static_cast<std::uint16_t>(MoveType::CAPTURE)
            );
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
        moves.emplace_back(
            static_cast<std::uint16_t>(from_mask(single_pawn)),
            static_cast<std::uint16_t>(*en_passant),
            static_cast<std::uint16_t>(MoveType::EN_PASSANT)
        );
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

std::uint64_t king_attackers(const Bitboard &bb, const AttackTable &at, const Colour colour) {
    const std::uint64_t king_pos { bb.colour_piece_mask(colour, KING) };
    const Square king_sq { from_mask(king_pos) };
    const Colour enemy_colour { opposite(colour) };
    const std::uint64_t occupied { bb.entire_mask() };
    const std::uint64_t enemies { bb.colour_mask(enemy_colour) };
    std::uint64_t king_attackers {};
    for (const Piece piece : ALL_PIECES) {
        const std::uint64_t piece_mask { bb.colour_piece_mask(enemy_colour, piece) };
        king_attackers |= (piece_mask & at.captures(king_sq, piece, colour, occupied, enemies));
    }
    return king_attackers;
}

/*
DecodedMove decode(const EncodedMove encoded_move) {
    move_type_v::Common common {
        static_cast<Square>(encoded_move.source_square),
        static_cast<Square>(encoded_move.dest_square),
        static_cast<Piece>(encoded_move.piece)
    };
    MoveType move_type { static_cast<MoveType>(encoded_move.move_type) };
    switch (move_type) {
        case MoveType::QUIET : return move_type_v::Quiet { common };
        case MoveType::CAPTURE : {
            assert(encoded_move.piece1 < NUM_PIECES);
            const Piece captured { static_cast<Piece>(encoded_move.piece1) }; 
            return move_type_v::Capture { common, captured };}
        case MoveType::DOUBLE_PAWN_PUSH : return move_type_v::DoublePawnPush { common };
        case MoveType::CASTLE_KINGSIDE : return move_type_v::CastleKingSide { common };
        case MoveType::CASTLE_QUEENSIDE : return move_type_v::CastleQueenSide { common };
        case MoveType::EN_PASSANT : return move_type_v::EnPassant { common };
        case MoveType::MOVE_PROMOTION : {
            assert(encoded_move.piece1 < NUM_PIECES);
            const Piece promoted { static_cast<Piece>(encoded_move.piece1) };
            return move_type_v::MovePromotion { common, promoted };}
        case MoveType::CAPTURE_PROMOTION : {
            assert(encoded_move.piece1 < NUM_PIECES);
            assert(encoded_move.piece2 < NUM_PIECES);
            const Piece promoted { static_cast<Piece>(encoded_move.piece1) };
            const Piece captured { static_cast<Piece>(encoded_move.piece2) };
            return move_type_v::CapturePromotion { common, promoted, captured };}
        default: throw std::invalid_argument("Unrecognised move type"); // should never happen
    }
}
*/