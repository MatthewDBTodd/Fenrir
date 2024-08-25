#include "board.h"
#include "fenrir_assert.h"
#include "move_parse.h"
#include "types.h"

#include <cctype>
#include <cmath>

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

std::optional<DecodedMove> parse_move_input(std::string_view input, const Board &board) {
    if (input.size() != 4 && input.size() != 5) {
        return std::nullopt;
    }

    const auto source { parse_square_string(input.substr(0, 2)) };
    if (!source.has_value()) {
        return std::nullopt;
    }

    const auto dest { parse_square_string(input.substr(2, 2)) };
    if (!dest.has_value()) {
        return std::nullopt;
    }

    const auto occupant { board.bitboard().square_occupant(*source) };
    if (!occupant.has_value()) {
        return std::nullopt;
    }

    const move_type_v::Common common { *source, *dest, occupant->second, occupant->first };

    const auto dest_occupant { board.bitboard().square_occupant(*dest) };

    if (dest_occupant.has_value() && dest_occupant->first == common.colour) {
        return std::nullopt;
    }

    if (input.size() == 5) {
        if (common.piece != PAWN) {
            return std::nullopt;
        }
        const auto promotion_piece { parse_piece_char(input[4]) };
        if (!promotion_piece.has_value() || 
            (*promotion_piece == PAWN || *promotion_piece == KING)) {
            return std::nullopt;
        }
        if (dest_occupant.has_value()) {
            return move_type_v::CapturePromotion { common, dest_occupant->second, *promotion_piece };
        } else {
            return move_type_v::MovePromotion { common, *promotion_piece };
        }
    } else if (common.piece == PAWN && 
               board.en_passant().has_value() && 
               common.dest == *board.en_passant()) {
        const std::optional<Square> pawn_square = [=]() -> std::optional<Square> {
            if (common.dest / 8 == 2) {
                return static_cast<Square>(common.dest + 8);
            } else if (common.dest / 8 == 5) {
                return static_cast<Square>(common.dest - 8);
            } else {
                return std::nullopt;
            }
        }();
        if (!pawn_square.has_value()) {
            return std::nullopt;
        }
        return move_type_v::EnPassant{ common, *pawn_square };
    } else if (dest_occupant.has_value()) {
        return move_type_v::Capture{ common, dest_occupant->second };
    } else if (common.piece == PAWN) {
        const auto difference { 
            std::abs(static_cast<int>(common.dest) - static_cast<int>(common.source)) 
        };
        if (difference == 8) {
            return move_type_v::Quiet { common };
        } else if (difference == 16) {
            const Square ep_square = static_cast<Square>(
                common.colour == WHITE ? common.dest - 8 : common.dest + 8
            );
            return move_type_v::DoublePawnPush { common, ep_square };
        } else {
            return std::nullopt;
        }
    } else if (common.piece == KING) {
        if (common.source == E1 && common.dest == G1) {
            return move_type_v::CastleKingSide { common };
        } else if (common.source == E1 && common.dest == C1) {
            return move_type_v::CastleQueenSide { common };
        } else if (common.source == E8 && common.dest == G8) {
            return move_type_v::CastleKingSide { common };
        } else if (common.source == E8 && common.dest == C8) {
            return move_type_v::CastleQueenSide { common };
        } else {
            return move_type_v::Quiet { common };
        }
    } else {
        return move_type_v::Quiet { common };
    }
}