#include <gtest/gtest.h>

#include "attack_table.h"
#include "bitboard.h"
#include "move_gen.h"
#include "test_helpers.h"
#include "types.h"

class TestMoveGen : public testing::Test {
protected:
    static const AttackTable at;
};

const AttackTable TestMoveGen::at {};

TEST_F(TestMoveGen, TestKingAttackers) {
    Bitboard bb { *Bitboard::from_fen("8/8/4r3/2q5/8/4K3/8/8") };
    EXPECT_EQ(mask_from_squares({ C5, E6 }), king_attackers(bb, at, WHITE));

    bb = *Bitboard::from_fen("8/3k4/4P3/3K4/8/8/8/8");
    EXPECT_EQ(mask_from_squares({ E6 }), king_attackers(bb, at, BLACK));

    bb = *Bitboard::from_fen("8/8/1k6/n7/8/1K6/8/8");
    EXPECT_EQ(mask_from_squares({ A5 }), king_attackers(bb, at, WHITE));

    bb = *Bitboard::from_fen("7k/8/8/8/8/1K6/8/B7");
    EXPECT_EQ(mask_from_squares({ A1 }), king_attackers(bb, at, BLACK));
}