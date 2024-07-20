#pragma once

#include "types.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
#include <ranges>
#include <string_view>

struct AttackSquaresTestCase {
    const Square square;
    const std::uint64_t attack_mask;
};

// Takes a fen string and converts it to a uint64_t representing its board layout
// We only care about whether squares are occupied/unoccupied so the actual pieces
// occupying the squares is irrelevant.
inline std::uint64_t fen_to_hex(std::string_view fen) {
    // we only care about the first section of the fen string.
    const auto end { std::find_if(fen.begin(), fen.end(), [](const char c) {
        return std::isspace(c);
    })};
    if (end == fen.end()) {
        throw std::exception();
    }
    fen = std::string_view(fen.begin(), end);

    std::uint64_t result {};
    int shift { 63 };
    auto first { fen.begin() };
    auto last { std::find(first, end, '/') };
    const auto fn = [&shift, &result](const std::string_view rank) {
        for (char c : std::views::reverse(rank)) {
            if (std::isdigit(c)) {
                const int n = c - '0';
                if (n > shift) {
                    break;
                }
                shift -= n;
            } else {
                result |= (1ull << shift);
                if (shift == 0) {
                    break;
                }
                shift -= 1;
            }
        }
    };
    while (last != fen.end()) {
        std::string_view rank(first, last);
        fn(rank);
        first = last+1;
        last = std::find(first, end, '/');
    }
    // final rank
    std::string_view rank(first, last);
    fn(rank);

    return result;
}

