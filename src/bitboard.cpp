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
    const std::uint64_t mask { 1ull << square };
    colours[colour] |= mask;
    pieces[piece] |= mask;
}

void Bitboard::remove_unchecked(const Colour colour, 
                                const Piece piece, 
                                const Square square) noexcept {
    const std::uint64_t mask { 1ull << square };
    colours[colour] ^= mask;
    pieces[piece] ^= mask;
}

void Bitboard::clear_unchecked(const Square square) noexcept {
    const std::uint64_t mask { 1ull << square };
    const auto mask_xor = [mask](const std::uint64_t n) { return n ^ mask; };
    std::transform(colours.begin(), colours.end(), colours.begin(), mask_xor);
    std::transform(pieces.begin(), pieces.end(), pieces.begin(), mask_xor);
}

char Bitboard::square_occupant(const Square square) const {
    const std::uint64_t mask { 1ull << square };
    if ((mask & entire_mask()) == 0) {
        return ' ';
    }
    // If this ends up being suboptimal I can optimise by storing an array of indexes
    // to the piece/colours array 
    const auto res { std::find_if(pieces.begin(), pieces.end(), 
        [this, mask](const std::uint64_t n) {
        return (mask & n) > 0;
    })};
    const Piece piece { static_cast<Piece>(std::distance(pieces.begin(), res)) };
    assert (piece < NUM_PIECES);
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
    if ((mask & colour_mask(WHITE)) > 0) {
        return std::toupper(piece_char);
    } else {
        return piece_char;
    }
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
            os << "| " << bb.square_occupant(static_cast<Square>(rank_idx + file))
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