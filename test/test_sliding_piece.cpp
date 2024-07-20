#include <gtest/gtest.h>

#include "sliding_piece.h"
#include "test_utility.h"

#include <memory>
#include <vector>

struct SlidingPieceTestCase {
    const Square square;
    std::string_view starting_pos;
    std::string_view expected_pos;
};

class TestSlidingPiece : public testing::Test {
protected:
    static const SlidingPieceAttacks attacks;
};

const SlidingPieceAttacks TestSlidingPiece::attacks {};

TEST_F(TestSlidingPiece, TestBishopAttacks) {
    const std::vector<SlidingPieceTestCase> test_cases {
        {
            B5,
            "r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 1",
            "8/8/B1B5/8/B1B5/3B4/4B3/5B2 w HAha - 0 1"
        }
    };

    for (const auto test_case : test_cases) {
        EXPECT_EQ(
            fen_to_hex(test_case.expected_pos),
            attacks.lookup(test_case.square, BISHOP, fen_to_hex(test_case.starting_pos))
        );
    }
}

TEST_F(TestSlidingPiece, TestRookAttacks) {
    const std::vector<SlidingPieceTestCase> test_cases {
        {
            E3,
            "8/4k3/8/4q3/8/4RK2/8/8 w - - 0 1",
            "8/8/8/4R3/4R3/RRRR1R2/4R3/4R3 w - - 0 1"
        }
    };

    for (const auto test_case : test_cases) {
        EXPECT_EQ(
            fen_to_hex(test_case.expected_pos),
            attacks.lookup(test_case.square, ROOK, fen_to_hex(test_case.starting_pos))
        );
    }
}
