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
    :   bitboard_(bb),
        fullmove_count_(fullmove_count),
        quiet_half_moves_(quiet_half_moves),
        turn_colour_(turn_colour),
        castling_(castling),
        en_passant_(en_passant)
{
    // prev_moves_.reserve(256);
}

void Board::make_move(const EncodedMove move) {
    return make_move(decode(move));
}

void Board::make_move(const DecodedMove &move) {
    // needs to be done before making the move as some of these values will get clobbered
    BOOST_ASSERT(back_ < prev_moves_.size());
    prev_moves_[back_++] = SavedMove {
        move,
        castling_,
        quiet_half_moves_,
        en_passant_
    };

    bitboard_.make_move(move);

    en_passant_ = std::nullopt;

    std::visit(*this, move);

    castling_.update_castling(move);

    // full move count gets incremented after blacks turn
    fullmove_count_ += turn_colour_;

    turn_colour_ = opposite(turn_colour_);
}

void Board::undo_last_move() {
    // BOOST_ASSERT(!prev_moves_.empty());
    BOOST_ASSERT(back_ > 0);
    back_ -= 1;
    const auto &last_move { prev_moves_[back_] }; 
    castling_ = last_move.prev_castling;
    quiet_half_moves_ = last_move.prev_quiet_half_moves;
    en_passant_ = last_move.prev_en_passant;

    turn_colour_ = opposite(turn_colour_);
    fullmove_count_ -= turn_colour_;

    bitboard_.unmake_move(last_move.move);
}

void Board::operator()(const move_type_v::Quiet &quiet) {
    if (quiet.common.piece != PAWN) {
        quiet_half_moves_ += 1;
    } else {
        quiet_half_moves_ = 0;
    }
}

void Board::operator()(const move_type_v::Capture &) {
    quiet_half_moves_ = 0;
}

void Board::operator()(const move_type_v::DoublePawnPush &dpp) {
    quiet_half_moves_ = 0;
    en_passant_ = dpp.ep_square;
}

void Board::operator()(const move_type_v::CastleKingSide &) {
    quiet_half_moves_ += 1;
}

void Board::operator()(const move_type_v::CastleQueenSide &) {
    quiet_half_moves_ += 1;
}

void Board::operator()(const move_type_v::EnPassant &) {
    quiet_half_moves_ = 0;
}

void Board::operator()(const move_type_v::MovePromotion &) {
    quiet_half_moves_ = 0;
}

void Board::operator()(const move_type_v::CapturePromotion &) {
    quiet_half_moves_ = 0;
}

static std::optional<Colour> turn_colour_from_fen(std::string_view fen);

// Bit ugly but en-passant is std::nullopt for when ep is not possible. So can't use
// nullopt for parsing error. When moving to C++ 23 can use std::expected<std::optional<Square>, ...>
struct InvalidEnPassantString {};
using EnPassantParseResult = std::variant<InvalidEnPassantString, std::optional<Square>>;

static EnPassantParseResult en_passant_from_fen(std::string_view fen);

template <std::integral T>
static std::optional<T> parse_int(std::string_view fen);

std::optional<Board> Board::init(std::string_view fen) {
    const std::vector<std::string_view> fen_parts { utility::split(fen, ' ') };
    // The first 4 parts we need no matter what, the final 2 fields is not the end of the world
    // if they're missing
    if (fen_parts.size() < 4) {
        return std::nullopt;
    }
    const std::optional<Bitboard> bb { Bitboard::from_fen(fen_parts[0]) };

    if (!bb.has_value()) {
        return std::nullopt;
    }

    const std::optional<Colour> turn_colour_ { 
        turn_colour_from_fen(fen_parts[1]) 
    };

    if (!turn_colour_.has_value()) {
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

    const std::optional<Square> en_passant_ {
        *std::get_if<std::optional<Square>>(&en_passant_result)
    };

    const std::uint8_t quiet_half_moves = [&] {
        if (fen_parts.size() > 4) {
            const auto parsed { parse_int<std::uint8_t>(fen_parts[4]) };
            if (parsed.has_value()) {
                return *parsed;
            } 
        }
        return std::uint8_t(0);
    }();

    const std::uint16_t fullmove_count = [&] {
        if (fen_parts.size() > 5) {
            const auto parsed { parse_int<std::uint16_t>(fen_parts[5]) };
            if (parsed.has_value()) {
                return *parsed;
            }
        }
        return std::uint16_t(1);
    }();

    return Board(*bb, fullmove_count, quiet_half_moves, *turn_colour_, 
                 *castling, en_passant_);
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