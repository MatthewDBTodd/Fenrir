#include "board.h"
#include "fenrir_assert.h"
#include "move_parse.h"
#include "types.h"

#include <cctype>
#include <cmath>
#include <iostream>

static std::optional<Square> parse_square_string(std::string_view input) {
    BOOST_ASSERT(input.size() == 2);
    const char file_char { static_cast<char>(std::tolower(input[0])) };
    const char rank_char { static_cast<char>(std::tolower(input[1])) };

    if (file_char < 'a' || file_char > 'h') {
        return std::nullopt;
    }
    if (rank_char < '1' || rank_char > '8') {
        return std::nullopt;
    }
    const auto file { file_char - 'a' };
    const auto rank { rank_char - '1' };
    return static_cast<Square>((rank * 8) + file);
}

static std::optional<Piece> parse_piece_char(char piece) {
    piece = std::tolower(piece);
    switch (piece) {
        case 'p': return PAWN;
        case 'n': return KNIGHT;
        case 'b': return BISHOP;
        case 'r': return ROOK;
        case 'q': return QUEEN;
        case 'k': return KING;
        default: return std::nullopt;
    }
}

std::optional<EncodedMove> parse_move_input(std::string_view input, const Board &board) {
    if (input.size() != 4 && input.size() != 5) {
        std::cerr << "Error: size of input move must be 4 or 5 with input: \"" << input << "\"\n";
        return std::nullopt;
    }

    const auto source { parse_square_string(input.substr(0, 2)) };
    if (!source.has_value()) {
        std::cerr << "Error: invalid source square with input: \"" << input << "\"\n";
        return std::nullopt;
    }

    const auto dest { parse_square_string(input.substr(2, 2)) };
    if (!dest.has_value()) {
        std::cerr << "Error: invalid dest square with input: \"" << input << "\"\n";
        return std::nullopt;
    }

    const auto occupant { board.bitboard().square_occupant(*source) };
    if (!occupant.has_value()) {
        std::cerr << "Error: source square is not occupied with input: \"" << input << "\"\n";
        return std::nullopt;
    }

    // const move_type_v::Common common { *source, *dest, occupant->second, occupant->first };
    const Colour colour { occupant->first };
    const Piece piece { occupant->second };

    const auto dest_occupant { board.bitboard().square_occupant(*dest) };

    if (dest_occupant.has_value() && dest_occupant->first == colour) {
        std::cerr << "Error: destination square is occupied by same colour with input: \"" << input << "\"\n";
        return std::nullopt;
    }

    if (input.size() == 5) {
        if (piece != PAWN) {
            std::cerr << "Error: size 5 input where piece != pawn with input: \"" << input << "\"\n";
            return std::nullopt;
        }
        const auto promotion_piece { parse_piece_char(input[4]) };
        if (!promotion_piece.has_value() || 
            (*promotion_piece == PAWN || *promotion_piece == KING)) {
            std::cerr << "Error: invalid promotion piece with input: \"" << input << "\"\n";
            return std::nullopt;
        }
        if (dest_occupant.has_value()) {
            return EncodedMove(MoveType::CAPTURE_PROMOTION, *source, *dest, piece,
                               colour, dest_occupant->second, *promotion_piece);
            // return move_type_v::CapturePromotion { common, dest_occupant->second, *promotion_piece };
        } else {
            return EncodedMove(MoveType::MOVE_PROMOTION, *source, *dest, piece,
                               colour, NUM_PIECES, *promotion_piece);
            // return move_type_v::MovePromotion { common, *promotion_piece };
        }
    } else if (piece == PAWN && 
               board.en_passant().has_value() && 
               *dest == *board.en_passant()) {
        /*
        const std::optional<Square> pawn_square = [=]() -> std::optional<Square> {
            if (*dest / 8 == 2) {
                return static_cast<Square>(*dest + 8);
            } else if (*dest / 8 == 5) {
                return static_cast<Square>(*dest - 8);
            } else {
                return std::nullopt;
            }
        }();
        if (!pawn_square.has_value()) {
            return std::nullopt;
        }
        */
        return EncodedMove(MoveType::EN_PASSANT, *source, *dest, piece,
                           colour, dest_occupant->second, NUM_PIECES);
        // return move_type_v::EnPassant{ common, *pawn_square };
    } else if (dest_occupant.has_value()) {
        return EncodedMove(MoveType::CAPTURE, *source, *dest, piece,
                           colour, dest_occupant->second, NUM_PIECES);
        // return move_type_v::Capture{ common, dest_occupant->second };
    } else if (piece == PAWN) {
        const auto difference { 
            std::abs(static_cast<int>(*dest) - static_cast<int>(*source)) 
        };
        if (difference == 8) {
            return EncodedMove(MoveType::QUIET, *source, *dest, piece,
                               colour, NUM_PIECES, NUM_PIECES);
            // return move_type_v::Quiet { common };
        } else if (difference == 16) {
            // const Square ep_square = static_cast<Square>(
            //     colour == WHITE ? *dest - 8 : *dest + 8
            // );
            return EncodedMove(MoveType::DOUBLE_PAWN_PUSH, *source, *dest, piece,
                               colour, NUM_PIECES, NUM_PIECES);
            // return move_type_v::DoublePawnPush { common, ep_square };
        } else {
            std::cerr << "Error: invalid pawn move with input: \"" << input << "\"\n";
            return std::nullopt;
        }
    } else if (piece == KING) {
        const auto move_type { [=] {
            if (*source == E1 && *dest == G1) {
                return MoveType::CASTLE_KINGSIDE;
            } else if (*source == E1 && *dest == C1) {
                return MoveType::CASTLE_QUEENSIDE;
            } else if (*source == E8 && *dest == G8) {
                return MoveType::CASTLE_KINGSIDE;
            } else if (*source == E8 && *dest == C8) {
                return MoveType::CASTLE_QUEENSIDE;
            } else {
                return MoveType::QUIET;
            }
        }() };
        return EncodedMove(move_type, *source, *dest, piece, colour,
                           NUM_PIECES, NUM_PIECES);
        // if (common.source == E1 && common.dest == G1) {
        //     return move_type_v::CastleKingSide { common };
        // } else if (common.source == E1 && common.dest == C1) {
        //     return move_type_v::CastleQueenSide { common };
        // } else if (common.source == E8 && common.dest == G8) {
        //     return move_type_v::CastleKingSide { common };
        // } else if (common.source == E8 && common.dest == C8) {
        //     return move_type_v::CastleQueenSide { common };
        // } else {
        //     return move_type_v::Quiet { common };
        // }
    } else {
        return EncodedMove(MoveType::QUIET, *source, *dest, occupant->second, occupant->first,
                           NUM_PIECES, NUM_PIECES);
        // return move_type_v::Quiet { common };
    }
}

std::string move_to_string(const EncodedMove move) {
    std::string rv;
    rv.append(1, 'a' + (move.source_square % 8));
    rv.append(1, '1' + (move.source_square / 8));
    rv.append(1, 'a' + (move.dest_square % 8));
    rv.append(1, '1' + (move.dest_square / 8 ));
    if (static_cast<MoveType>(move.move_type) == MoveType::MOVE_PROMOTION || 
        static_cast<MoveType>(move.move_type) == MoveType::CAPTURE_PROMOTION) {

        rv.append(1, [=]{
            switch (static_cast<Piece>(move.promoted_piece)) {
                case KNIGHT: return 'n';
                case BISHOP: return 'b';
                case ROOK: return 'r';
                case QUEEN: return 'q';
                default: return 'x'; // should not happen but here to make it obvious if it does
            }
        }());
    }
    return rv;
}