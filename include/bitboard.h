#pragma once

#include "types.h"

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

// Any function with "unchecked" in the name means there's no checking of the validity
// of the operation. E.g. it doesn't check if the requested square is already occupied
// when calling place_unchecked.

class Bitboard {
public:
    static std::optional<Bitboard> from_fen(std::string_view pieces);

    void place_unchecked(const Colour colour, 
                         const Piece piece, 
                         const Square square) noexcept;

    void remove_unchecked(const Colour colour, 
                          const Piece piece, 
                          const Square square) noexcept;

    void clear_unchecked(const Square square) noexcept;

    std::uint64_t colour_mask(const Colour colour) const noexcept {
        return colours[colour];
    }

    std::uint64_t piece_mask(const Piece piece) const noexcept {
        return pieces[piece];
    }

    std::uint64_t colour_piece_mask(const Colour colour, const Piece piece) const noexcept {
        return colours[colour] & pieces[piece];
    }

    std::uint64_t entire_mask() const noexcept {
        return colours[WHITE] | colours[BLACK];
    }

    bool square_empty(Square square) {
        return static_cast<bool>(entire_mask() & (1 << square));
    }

private:
    std::array<std::uint64_t, NUM_COLOURS> colours {};
    std::array<std::uint64_t, NUM_PIECES> pieces {};
};
