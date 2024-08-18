#include "board.h"
#include "castling.h"
#include "utility.h"
#include "types.h"

#include <iostream>
#include <limits>
#include <concepts>
#include <string>

Board::Board(const Bitboard &bb, const std::uint16_t fullmove_count,
             const std::uint16_t quiet_half_moves, const Colour turn_colour,
             const CastlingRights castling, const std::optional<Square> en_passant)
    :   bitboard(bb),
        fullmove_count(fullmove_count),
        quiet_half_moves(quiet_half_moves),
        turn_colour(turn_colour),
        castling(castling),
        en_passant(en_passant)
{}

void Board::make_move(const DecodedMove &move) {
    // needs to be done before making the move as some of these values will get clobbered
    prev_moves.emplace_back(SavedMove {
        move,
        castling,
        quiet_half_moves,
        en_passant
    });

    bitboard.make_move(move);

    en_passant = std::nullopt;

    std::visit(*this, move);

    castling.update_castling(move);

    // full move count gets incremented after blacks turn
    fullmove_count += turn_colour;

    turn_colour = opposite(turn_colour);
}

void Board::undo_last_move() {
    BOOST_ASSERT(!prev_moves.empty());
    const auto &last_move { prev_moves.back() }; 
    castling = last_move.prev_castling;
    quiet_half_moves = last_move.prev_quiet_half_moves;
    en_passant = last_move.prev_en_passant;

    turn_colour = opposite(turn_colour);
    fullmove_count -= turn_colour;

    bitboard.unmake_move(last_move.move);

    prev_moves.pop_back();
}

void Board::operator()(const move_type_v::Quiet &quiet) {
    if (quiet.common.piece != PAWN) {
        quiet_half_moves += 1;
    } else {
        quiet_half_moves = 0;
    }
}

void Board::operator()(const move_type_v::Capture &) {
    quiet_half_moves = 0;
}

void Board::operator()(const move_type_v::DoublePawnPush &dpp) {
    quiet_half_moves = 0;
    en_passant = dpp.ep_square;
}

void Board::operator()(const move_type_v::CastleKingSide &) {
    quiet_half_moves += 1;
}

void Board::operator()(const move_type_v::CastleQueenSide &) {
    quiet_half_moves += 1;
}

void Board::operator()(const move_type_v::EnPassant &) {
    quiet_half_moves = 0;
}

void Board::operator()(const move_type_v::MovePromotion &) {
    quiet_half_moves = 0;
}

void Board::operator()(const move_type_v::CapturePromotion &) {
    quiet_half_moves = 0;
}

static std::optional<Colour> turn_colour_from_fen(std::string_view fen);

// Bit ugly but en-passant is std::nullopt for when ep is not possible. So can't use
// nullopt for parsing error. When moving to C++ 23 can use std::expected<std::optional<Square>, ...>
struct InvalidEnPassantString {};
using EnPassantParseResult = std::variant<InvalidEnPassantString, std::optional<Square>>;

static EnPassantParseResult en_passant_from_fen(std::string_view fen);

template <std::integral T>
static std::optional<T> parse_int(std::string_view fen);

std::optional<Board> Board::from_fen(std::string_view fen) {
    const std::vector<std::string_view> fen_parts { utility::split(fen, ' ') };
    if (fen_parts.size() != 6) {
        return std::nullopt;
    }
    const std::optional<Bitboard> bb { Bitboard::from_fen(fen_parts[0]) };

    if (!bb.has_value()) {
        return std::nullopt;
    }

    const std::optional<Colour> turn_colour { 
        turn_colour_from_fen(fen_parts[1]) 
    };

    if (!turn_colour.has_value()) {
        return std::nullopt;
    }

    const std::optional<CastlingRights> castling { 
        CastlingRights::from_fen(fen_parts[2]) 
    };

    if (!castling.has_value()) {
        return std::nullopt;
    }

    const auto en_passant_result { en_passant_from_fen(fen_parts[3]) };

    if (std::holds_alternative<InvalidEnPassantString>(en_passant_result)) {
        return std::nullopt;
    }

    const std::optional<Square> en_passant {
        *std::get_if<std::optional<Square>>(&en_passant_result)
    };

    const std::optional<std::uint8_t> quiet_half_moves { 
        parse_int<std::uint8_t>(fen_parts[4]) 
    };

    if (!quiet_half_moves.has_value()) {
        return std::nullopt;
    }

    const std::optional<std::uint16_t> fullmove_count {
        parse_int<std::uint16_t>(fen_parts[5])
    };

    if (!fullmove_count.has_value()) {
        return std::nullopt;
    }

    return Board(*bb, *fullmove_count, *quiet_half_moves, *turn_colour, 
                 *castling, en_passant);
}

static std::optional<Colour> turn_colour_from_fen(std::string_view fen) {
    if (fen == "w") {
        return WHITE;
    } else if (fen == "b") {
        return BLACK;
    } else {
        return std::nullopt;
    }
}
static EnPassantParseResult en_passant_from_fen(std::string_view fen) {
    if (fen == "-") {
        return std::nullopt;
    }

    if (fen.size() > 2) {
        std::cerr << "FEN ERROR: En-passant string too long\n";
        return InvalidEnPassantString {};
    }

    if (fen[0] < 'a' || fen[0] > 'h') {
        std::cerr << "FEN ERROR: Invalid file: " << fen[0] << "\n";
        return InvalidEnPassantString {};
    }

    if (fen[1] < '1' || fen[1] > '8') {
        std::cerr << "FEN ERROR: Invalid rank: " << fen[1] << "\n";
        return InvalidEnPassantString {};
    }

    const auto file { fen[0] - 'a' };
    const auto rank { fen[1] - '1' };

    return static_cast<Square>((file % 8) + (rank * 8));
}

template <std::integral T>
static std::optional<T> parse_int(std::string_view fen) {
    try {
        const int val { std::stoi(std::string(fen)) };

        if (val < std::numeric_limits<T>::min() || val > std::numeric_limits<T>::max()) {
            return std::nullopt;
        }

        return static_cast<T>(val);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return std::nullopt;
    }
}