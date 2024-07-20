#include "bitboard.h"

#include <algorithm>

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
