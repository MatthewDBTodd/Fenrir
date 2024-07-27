#include <gtest/gtest.h>

#include "sliding_piece.h"
#include "test_helpers.h"

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
        },
        {
            B2,
            "r1bq1rk1/ppp2ppp/1bn2n2/3p4/1P2p3/P3P1P1/1BPPNPBP/RN1Q1RK1 w Qq - 0 1",
            "8/8/5B2/4B3/3B4/B1B5/8/B1B5 w - - 0 1"
        },
        {
            G2,
            "r1bq1rk1/ppp2ppp/1bn2n2/3p4/1P2p3/P3P1P1/1BPPNPBP/RN1Q1RK1 w Qq - 0 1",
            "8/8/8/8/4B3/5B1B/8/5B1B w - - 0 1"
        },
        {
            C8,
            "r1bq1rk1/ppp2ppp/1bn2n2/3p4/1P2p3/P3P1P1/1BPPNPBP/RN1Q1RK1 w Qq - 0 1",
            "8/1b1b4/4b3/5b2/6b1/7b/8/8 w - - 0 1" 
        },
        {
            B6,
            "r1bq1rk1/ppp2ppp/1bn2n2/3p4/1P2p3/P3P1P1/1BPPNPBP/RN1Q1RK1 w Qq - 0 1",
            "8/b1b5/8/b1b5/3b4/4b3/8/8 w - - 0 1"
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
        },
        {
            B3,
            "r5k1/pbpq1ppp/1pnp1n2/4r3/2P1p3/BRPPPNP1/P1Q2PBP/4R1K1 w q - 0 1",
            "8/8/1R6/1R6/1R6/R1R5/1R6/1R6 w - - 0 1"
        },
        {
            E1,
            "r5k1/pbpq1ppp/1pnp1n2/4r3/2P1p3/BRPPPNP1/P1Q2PBP/4R1K1 w q - 0 1",
            "8/8/8/8/8/4R3/4R3/RRRR1RR1 w - - 0 1"
        },
        {
            A8,
            "r5k1/pbpq1ppp/1pnp1n2/4r3/2P1p3/BRPPPNP1/P1Q2PBP/4R1K1 w q - 0 1",
            "1rrrrrr1/r7/8/8/8/8/8/8 w - - 0 1"
        },
        {
            E5,
            "r5k1/pbpq1ppp/1pnp1n2/4r3/2P1p3/BRPPPNP1/P1Q2PBP/4R1K1 w q - 0 1"    ,
            "4r3/4r3/4r3/rrrr1rrr/4r3/8/8/8 w - - 0 1"
        }
    };

    for (const auto test_case : test_cases) {
        EXPECT_EQ(
            fen_to_hex(test_case.expected_pos),
            attacks.lookup(test_case.square, ROOK, fen_to_hex(test_case.starting_pos))
        );
    }
}
