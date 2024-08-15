#include "bitboard.h"
#include "utility.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>
#include <iterator>
#include <ranges>

std::optional<Bitboard> Bitboard::from_fen(std::string_view pieces) {
    const std::vector<std::string_view> ranks { utility::split(pieces, '/') };
    if (ranks.size() != 8) {
        std::cerr << "Invalid FEN String: not enough ranks in the FEN string\n";
        return std::nullopt;
    }
    Bitboard bb {};

    int rank_offset { 64 };
    for (const auto rank : ranks) {
        rank_offset -= 8;
        int i { 0 };
        for (const char c : rank) {
            if (std::isalpha(c)) {
                const Colour colour { std::isupper(c) ? WHITE : BLACK };
                const std::optional<Piece> piece = [](const char c) -> std::optional<Piece> {
                    switch (std::tolower(c)) {
                        case 'p': return PAWN;
                        case 'n': return KNIGHT;
                        case 'b': return BISHOP;
                        case 'r': return ROOK;
                        case 'q': return QUEEN;
                        case 'k': return KING;
                        default: return std::nullopt;
                    }
                } (c);
                if (!piece) {
                    std::cerr << "Invalid FEN String: unknown piece type '" << c << "'\n" ;
                    return std::nullopt;
                }
                bb.place_unchecked(colour, *piece, static_cast<Square>(rank_offset + i));
                i += 1;
            } else if ('1' <= c && c <= '8') {
                i += (c - '0');
            } else {
                std::cerr << "Invalid FEN String: invalid character '" << c << "'\n";
                return std::nullopt;
            }
            if (i > 8) {
                std::cerr << "Invalid FEN String: too many pieces in rank\n";
                return std::nullopt;
            }
        }
    }    
    return bb;
}

void Bitboard::place_unchecked(const Colour colour, const Piece piece, 
                               const Square square) noexcept {
    const std::uint64_t mask { 1ul << square };
    colours[colour] |= mask;
    pieces[piece] |= mask;
}

void Bitboard::remove_unchecked(const Colour colour, 
                                const Piece piece, 
                                const Square square) noexcept {
    const std::uint64_t mask { 1ul << square };
    colours[colour] ^= mask;
    pieces[piece] ^= mask;
}

void Bitboard::clear_unchecked(const Square square) noexcept {
    const std::uint64_t mask { 1ul << square };
    const auto mask_xor = [=](const std::uint64_t n) { return n ^ mask; };
    std::transform(colours.begin(), colours.end(), colours.begin(), mask_xor);
    std::transform(pieces.begin(), pieces.end(), pieces.begin(), mask_xor);
}

std::optional<std::pair<Colour, Piece>> Bitboard::square_occupant(const Square square) const {
    const std::uint64_t mask { from_square(square) };
    if ((mask & entire_mask()) == 0) {
        return std::nullopt;
    }

    const Colour colour { mask & colour_mask(WHITE) ? WHITE : BLACK };

    // If this ends up being suboptimal I can optimise by storing an array of indexes
    // to the piece/colours array 
    const auto res { std::find_if(pieces.begin(), pieces.end(), 
        [=](const std::uint64_t n) {
        return (mask & n) > 0;
    })};
    const Piece piece { static_cast<Piece>(std::distance(pieces.begin(), res)) };
    assert(piece < NUM_PIECES);
    return std::make_pair(colour, piece);
}

char Bitboard::square_representation(const Square square) const {
    const auto occupant { square_occupant(square) };
    if (!occupant.has_value()) {
        return ' ';
    }
    const Piece piece { occupant->second };
    const char piece_char = [piece] {
        switch (piece) {
            case PAWN: return 'p';
            case KNIGHT: return 'n';
            case BISHOP: return 'b';
            case ROOK: return 'r';
            case QUEEN: return 'q';
            case KING: return 'k';
            default: return ' ';
        }
    }();

    const Colour colour { occupant->first };
    if (colour == WHITE) {
        return std::toupper(piece_char);
    } else {
        return piece_char;
    }
}

void Bitboard::make_move(const DecodedMove &move) {
    std::visit(*this, move, MoveAction { move_action_v::Make {} });
}

void Bitboard::unmake_move(const DecodedMove &move) {
    std::visit(*this, move, MoveAction { move_action_v::UnMake {} });
}

void Bitboard::operator()(const move_type_v::Quiet quiet, move_action_v::Make) {
    remove_unchecked(quiet.common.colour, quiet.common.piece, quiet.common.source);
    place_unchecked(quiet.common.colour, quiet.common.piece, quiet.common.dest);
}

void Bitboard::operator()(const move_type_v::Quiet quiet, move_action_v::UnMake) {
    remove_unchecked(quiet.common.colour, quiet.common.piece, quiet.common.dest);
    place_unchecked(quiet.common.colour, quiet.common.piece, quiet.common.source);
}

// cppcheck-suppress passedByValue
void Bitboard::operator()(const move_type_v::Capture cap, move_action_v::Make) {
    remove_unchecked(cap.common.colour, cap.common.piece, cap.common.source);
    remove_unchecked(opposite(cap.common.colour), cap.captured_piece, cap.common.dest);
    place_unchecked(cap.common.colour, cap.common.piece, cap.common.dest);
}

// cppcheck-suppress passedByValue
void Bitboard::operator()(const move_type_v::Capture cap, move_action_v::UnMake) {
    remove_unchecked(cap.common.colour, cap.common.piece, cap.common.dest);
    place_unchecked(cap.common.colour, cap.common.piece, cap.common.source);
    place_unchecked(opposite(cap.common.colour), cap.captured_piece, cap.common.dest);
}

// cppcheck-suppress passedByValue
void Bitboard::operator()(const move_type_v::DoublePawnPush dpp, move_action_v::Make) {
    remove_unchecked(dpp.common.colour, dpp.common.piece, dpp.common.source);
    place_unchecked(dpp.common.colour, dpp.common.piece, dpp.common.dest);
}

// cppcheck-suppress passedByValue
void Bitboard::operator()(const move_type_v::DoublePawnPush dpp, move_action_v::UnMake) {
    remove_unchecked(dpp.common.colour, dpp.common.piece, dpp.common.dest);
    place_unchecked(dpp.common.colour, dpp.common.piece, dpp.common.source);
}

void Bitboard::operator()(const move_type_v::CastleKingSide cks, move_action_v::Make) {
    remove_unchecked(cks.common.colour, cks.common.piece, cks.common.source);
    place_unchecked(cks.common.colour, cks.common.piece, cks.common.dest);
    const Square rook_source = [=] {
        if (cks.common.colour == WHITE) {
            return H1;
        } else {
            return H8;
        }
    }();
    const Square rook_dest = [=] {
        if (cks.common.colour == WHITE) {
            return F1;
        } else {
            return F8;
        }
    }();
    remove_unchecked(cks.common.colour, ROOK, rook_source);
    place_unchecked(cks.common.colour, ROOK, rook_dest);
}

void Bitboard::operator()(const move_type_v::CastleKingSide cks, move_action_v::UnMake) {
    remove_unchecked(cks.common.colour, cks.common.piece, cks.common.dest);
    place_unchecked(cks.common.colour, cks.common.piece, cks.common.source);
    const Square rook_source = [=] {
        if (cks.common.colour == WHITE) {
            return H1;
        } else {
            return H8;
        }
    }();
    const Square rook_dest = [=] {
        if (cks.common.colour == WHITE) {
            return F1;
        } else {
            return F8;
        }
    }();
    remove_unchecked(cks.common.colour, ROOK, rook_dest);
    place_unchecked(cks.common.colour, ROOK, rook_source);
}

void Bitboard::operator()(const move_type_v::CastleQueenSide cqs, move_action_v::Make) {
    remove_unchecked(cqs.common.colour, cqs.common.piece, cqs.common.source);
    place_unchecked(cqs.common.colour, cqs.common.piece, cqs.common.dest);
    const Square rook_source = [=] {
        if (cqs.common.colour == WHITE) {
            return A1;
        } else {
            return A8;
        }
    }();
    const Square rook_dest = [=] {
        if (cqs.common.colour == WHITE) {
            return D1;
        } else {
            return D8;
        }
    }();
    remove_unchecked(cqs.common.colour, ROOK, rook_source);
    place_unchecked(cqs.common.colour, ROOK, rook_dest);
}

void Bitboard::operator()(const move_type_v::CastleQueenSide cqs, move_action_v::UnMake) {
    remove_unchecked(cqs.common.colour, cqs.common.piece, cqs.common.dest);
    place_unchecked(cqs.common.colour, cqs.common.piece, cqs.common.source);
    const Square rook_source = [=] {
        if (cqs.common.colour == WHITE) {
            return A1;
        } else {
            return A8;
        }
    }();
    const Square rook_dest = [=] {
        if (cqs.common.colour == WHITE) {
            return D1;
        } else {
            return D8;
        }
    }();
    remove_unchecked(cqs.common.colour, ROOK, rook_dest);
    place_unchecked(cqs.common.colour, ROOK, rook_source);
}

// cppcheck-suppress passedByValue
void Bitboard::operator()(const move_type_v::EnPassant ep, move_action_v::Make) {
    remove_unchecked(ep.common.colour, ep.common.piece, ep.common.source);
    place_unchecked(ep.common.colour, ep.common.piece, ep.common.dest);
    remove_unchecked(opposite(ep.common.colour), PAWN, ep.pawn_square);
}

// cppcheck-suppress passedByValue
void Bitboard::operator()(const move_type_v::EnPassant ep, move_action_v::UnMake) {
    remove_unchecked(ep.common.colour, ep.common.piece, ep.common.dest);
    place_unchecked(ep.common.colour, ep.common.piece, ep.common.source);
    place_unchecked(opposite(ep.common.colour), PAWN, ep.pawn_square);
}

// cppcheck-suppress passedByValue
void Bitboard::operator()(const move_type_v::MovePromotion mp, move_action_v::Make) {
    remove_unchecked(mp.common.colour, mp.common.piece, mp.common.source);
    place_unchecked(mp.common.colour, mp.promotion_piece, mp.common.dest);
}

// cppcheck-suppress passedByValue
void Bitboard::operator()(const move_type_v::MovePromotion mp, move_action_v::UnMake) {
    remove_unchecked(mp.common.colour, mp.promotion_piece, mp.common.dest);
    place_unchecked(mp.common.colour, mp.common.piece, mp.common.source);
}

// cppcheck-suppress passedByValue
void Bitboard::operator()(const move_type_v::CapturePromotion cp, move_action_v::Make) {
    remove_unchecked(cp.common.colour, cp.common.piece, cp.common.source);
    remove_unchecked(opposite(cp.common.colour), cp.captured_piece, cp.common.dest);
    place_unchecked(cp.common.colour, cp.promotion_piece, cp.common.dest);
}

// cppcheck-suppress passedByValue
void Bitboard::operator()(const move_type_v::CapturePromotion cp, move_action_v::UnMake) {
    remove_unchecked(cp.common.colour, cp.promotion_piece, cp.common.dest);
    place_unchecked(cp.common.colour, cp.common.piece, cp.common.source);
    place_unchecked(opposite(cp.common.colour), cp.captured_piece, cp.common.dest);
}

bool operator==(const Bitboard &a, const Bitboard &b) {
    return a.colours == b.colours && a.pieces == b.pieces;
}

bool operator!=(const Bitboard &a, const Bitboard &b) {
    return !(a == b);
}

// TODO - this can probably be less ugly
std::ostream& operator<<(std::ostream &os, const Bitboard &bb) {
    os << std::endl;
    std::size_t rank_idx { 64 };
    for (const int rank : std::views::iota(1, 9) | std::views::reverse) {
        rank_idx -= 8;
        os << "   ";
        for (const int _ : std::views::iota(0, 8)) {
            os << "+---";
        }
        os << std::endl;
        os << " " << rank << " ";
        for (const int file : std::views::iota(0, 8)) {
            os << "| " << bb.square_representation(static_cast<Square>(rank_idx + file))
               << " ";
        }
        os << "|" << std::endl;
    }
    os << "   ";
    for (const int _ : std::views::iota(0, 8)) {
        os << "+---";
    }
    os << std::endl;
    os << "     a   b   c   d   e   f   g   h" << std::endl;;
    os << std::endl;

    return os;
}