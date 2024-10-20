#pragma once

#include "types.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
#include <iostream>
#include <ranges>
#include <string_view>
#include <vector>

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
                result |= (1ul << shift);
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


inline std::uint64_t mask_from_squares(const std::vector<Square> &occupied_squares) {
    const bool has_duplicates = std::adjacent_find(
        occupied_squares.begin(), occupied_squares.end()
    ) != occupied_squares.end();
    if (has_duplicates) {
        std::cerr << "Occupied squares has duplicates. This is probably a mistake\n";
        throw std::exception();
    }
    std::uint64_t rv {};
    for (const Square sq : occupied_squares) { rv |= (1ul << sq); }
    return rv;
}

class MaskDisplay {
public:
    explicit MaskDisplay(const std::uint64_t mask) : mask_(mask) {}
    inline friend std::ostream& operator<<(std::ostream &os, const MaskDisplay mask);
private:
    const std::uint64_t mask_;
};

inline std::ostream& operator<<(std::ostream &os, const MaskDisplay mask) {
    os << std::endl;
    std::size_t rank_idx { 64 };
    for (const int rank : std::views::iota(1, 9) | std::views::reverse) {
        rank_idx -= 8;
        os << "   ";
        for (const int _ : std::views::iota(0, 8)) {
            os << "+---";
            (void)_;
        }
        os << std::endl;
        os << " " << rank << " ";
        for (const int file : std::views::iota(0, 8)) {
            os << "| " << (mask.mask_ & (1ul << (rank_idx + file)) ? "X" : " ")
               << " ";
        }
        os << "|" << std::endl;
    }
    os << "   ";
    for (const int _ : std::views::iota(0, 8)) {
        os << "+---";
        (void)_;
    }
    os << std::endl;
    os << "     a   b   c   d   e   f   g   h" << std::endl;;
    os << std::endl;

    return os;
}