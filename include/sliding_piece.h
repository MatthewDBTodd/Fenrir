#pragma once

#include "types.h"

#include <cstdint>
#include <vector>

class Magic {
public:
    static Magic init(const Square square, const Piece piece);
    std::uint64_t get_attacks(const std::uint64_t occupied_unmasked);
private:
    Magic(const std::uint64_t magic, const std::uint64_t mask, const int shift,
          std::vector<std::uint64_t> attacks);

    const std::uint64_t magic;
    const std::uint64_t mask;
    const int shift;
    const std::vector<std::uint64_t> attacks;
};
