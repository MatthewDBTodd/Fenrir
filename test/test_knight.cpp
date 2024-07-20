#include <gtest/gtest.h>

#include "knight.h"
#include "test_utility.h"

#include <vector>

class TestKnight : public testing::Test {
protected:
    static constexpr std::array<std::uint64_t, NUM_SQUARES> knight_attacks {
        piece::generate_knight_att_squares()
    };
};

TEST_F(TestKnight, TestKnightAttacks) {
    const std::vector<AttackSquaresTestCase> test_cases {
        { D4, 0x142200221400 },
        { D1, 0x142200 },
        { D2, 0x14220022 },
        { D7, 0x2200221400000000 },
        { D8, 0x22140000000000 },
        { A1, 0x20400 },
        { B1, 0x50800 },
        { A2, 0x2040004 },
        { A3, 0x204000402 },
        { B2, 0x5080008 },
        { A8, 0x4020000000000 },
        { A7, 0x400040200000000 },
        { A6, 0x204000402000000 },
        { B8, 0x8050000000000 },
        { B7, 0x800080500000000 },
        { H1, 0x402000 },
        { H2, 0x40200020 },
        { G1, 0xa01000 },
        { G2, 0xa0100010 },
        { H5, 0x40200020400000 },
        { G5, 0xa0100010a00000 },
        { H8, 0x20400000000000 },
        { H7, 0x2000204000000000 },
        { G7, 0x100010a000000000 },
        { G8, 0x10a00000000000 },
    };

    for (const auto test_case : test_cases) {
        EXPECT_EQ(test_case.attack_mask, knight_attacks[test_case.square]);
    }
}
