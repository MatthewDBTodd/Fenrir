#include <gtest/gtest.h>

#include "king.h"
#include "test_utility.h"

#include <vector>

class TestKing : public testing::Test {
protected:
    static constexpr std::array<std::uint64_t, NUM_SQUARES> king_attacks {
        piece::generate_king_att_squares()
    };
};

TEST_F(TestKing, TestKingAttacks) {
    const std::vector<AttackSquaresTestCase> test_cases {
        { D4, 0x1c141c0000 },
        { A4, 0x302030000 },
        { A1, 0x302 },
        { A8, 0x203000000000000 },
        { D1, 0x1c14 },
        { D8, 0x141c000000000000 },
        { H1, 0xc040 },
        { H4, 0xc040c00000 },
        { H8, 0x40c0000000000000 },
    };

    for (const auto test_case : test_cases) {
        EXPECT_EQ(test_case.attack_mask, king_attacks[test_case.square]);
    }
}
