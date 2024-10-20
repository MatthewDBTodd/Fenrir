#pragma once

#include "types.h"

#include <array>
#include <cstdint>
#include <vector>

class Magic {
public:
    static Magic init(const Square square, const Piece piece);
    std::uint64_t get_attacks(const std::uint64_t occupied_unmasked) const;
#ifdef FENRIR_PROFILING
    std::size_t size_bytes() const {
        return attacks.size() * sizeof(attacks[0]);
    }
#endif
private:
    Magic(const std::uint64_t magic, const std::uint64_t mask, const int shift,
          std::vector<std::uint64_t> attacks);

    const std::uint64_t magic {};
    const std::uint64_t mask {};
    const int shift {};
    const std::vector<std::uint64_t> attacks;
};

class SlidingPieceAttacks {
public:
    SlidingPieceAttacks();
    std::uint64_t lookup(const Square square, const Piece piece, 
                         const std::uint64_t blockers) const;
private:
    const std::array<Magic, NUM_SQUARES> rook_attacks;
    const std::array<Magic, NUM_SQUARES> bishop_attacks;
};
