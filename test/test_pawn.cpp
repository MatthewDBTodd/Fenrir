#include <gtest/gtest.h>

#include "pawn.h"

#include <vector>

struct PawnTestCase {
    const Square square;
    const std::uint64_t white_mask;
    const std::uint64_t black_mask;
};

class TestPawn : public testing::Test {
protected:
    static constexpr piece::PawnAttackTable pawn_attacks {};
};

TEST_F(TestPawn, TestPawnMoves) {
    const std::vector<PawnTestCase> test_cases {
        { D4, 0x800000000, 0x80000 },
        { D2, 0x8080000, 0x8 },
        { A7, 0x100000000000000, 0x10100000000 },
        { D1, 0x0, 0x0 },
        { G8, 0x0, 0x0 },
    };

    for (const auto test_case : test_cases) {
        EXPECT_EQ(test_case.white_mask, 
                  pawn_attacks.lookup(WHITE, piece::PawnAttackTable::MOVE, 
                                      test_case.square));
        EXPECT_EQ(test_case.black_mask,
                  pawn_attacks.lookup(BLACK, piece::PawnAttackTable::MOVE,
                                      test_case.square));
    }
}

TEST_F(TestPawn, TestPawnAttacks) {
    const std::vector<PawnTestCase> test_cases {
        { E5, 0x280000000000, 0x28000000 },
        { A2, 0x20000, 0x2 },
        { H5, 0x400000000000, 0x40000000 },
        { B1, 0x0, 0x0 },
    };

    for (const auto test_case : test_cases) {
        EXPECT_EQ(test_case.white_mask,
                  pawn_attacks.lookup(WHITE, piece::PawnAttackTable::ATTACK,
                                      test_case.square));
        EXPECT_EQ(test_case.black_mask,
                  pawn_attacks.lookup(BLACK, piece::PawnAttackTable::ATTACK,
                                      test_case.square));
    }
}