#pragma once

#include "direction.h"
#include "types.h"

#include <array>
#include <cstdint>
#include <utility>

namespace {

constexpr std::uint64_t SECOND_RANK {
      (1ull << A2) | (1ull << B2) | (1ull << C2) | (1ull << D2)
    | (1ull << E2) | (1ull << F2) | (1ull << G2) | (1ull << H2)
};

constexpr std::uint64_t SEVENTH_RANK {
      (1ull << A7) | (1ull << B7) | (1ull << C7) | (1ull << D7)
    | (1ull << E7) | (1ull << F7) | (1ull << G7) | (1ull << H7)
};

constexpr std::array<std::uint64_t, NUM_SQUARES> gen_attacks(const Colour colour) {
    std::array<std::uint64_t, NUM_SQUARES> attacks {};
    std::uint64_t mask { 1ull << A2 };
    for (std::size_t i = A2; i < A8; ++i) {
        if (colour == WHITE) {
            attacks[i] = direction::north_east(mask) 
                       | direction::north_west(mask);
        } else {
            attacks[i] = direction::south_east(mask)
                       | direction::south_west(mask) ;
        }
        mask <<= 1;
    }
    return attacks;
}

constexpr std::array<std::uint64_t, NUM_SQUARES> gen_moves(const Colour colour) {
    std::array<std::uint64_t, NUM_SQUARES> moves {};
    std::uint64_t mask { 1ull << A2 };
    for (std::size_t i = A2; i < A8; ++i) {
        if (colour == WHITE) {
            std::uint64_t result { direction::north(mask) };
            if (mask & SECOND_RANK) {
                result |= direction::north(direction::north(mask));
            }
            moves[i] = result;
        } else {
            std::uint64_t result { direction::south(mask) };
            if (mask & SEVENTH_RANK) {
                result |= direction::south(direction::south(mask));
            }
            moves[i] = result;
        }
        mask <<= 1;
    }
    return moves;
}

} // namespace

namespace piece {

class PawnAttackTable {
private:
    struct AttackMoves {
        constexpr AttackMoves(const Colour colour) :
            attacks(gen_attacks(colour)),
            moves(gen_moves(colour)) {}

        const std::array<std::uint64_t, NUM_SQUARES> attacks;
        const std::array<std::uint64_t, NUM_SQUARES> moves;
    };

public:
    std::array<AttackMoves, NUM_COLOURS> colour;

    constexpr PawnAttackTable() :
        colour{AttackMoves{WHITE}, AttackMoves{BLACK}} {}
};

} // namespace piece
