#include <gtest/gtest.h>

#include "attack_table.h"
#include "bitboard.h"
#include "castling.h"
#include "move_gen.h"
#include "test_helpers.h"
#include "types.h"

class TestMoveGen : public testing::Test {
protected:
    static const AttackTable at;
};

const AttackTable TestMoveGen::at {};

// TEST_F(TestMoveGen, TestKingAttackers) {
//     Bitboard bb { *Bitboard::from_fen("8/8/4r3/2q5/8/4K3/8/8") };
//     EXPECT_EQ(mask_from_squares({ C5, E6 }), king_attackers(bb, at, WHITE));
// 
//     bb = *Bitboard::from_fen("8/3k4/4P3/3K4/8/8/8/8");
//     EXPECT_EQ(mask_from_squares({ E6 }), king_attackers(bb, at, BLACK));
// 
//     bb = *Bitboard::from_fen("8/8/1k6/n7/8/1K6/8/8");
//     EXPECT_EQ(mask_from_squares({ A5 }), king_attackers(bb, at, WHITE));
// 
//     bb = *Bitboard::from_fen("7k/8/8/8/8/1K6/8/B7");
//     EXPECT_EQ(mask_from_squares({ A1 }), king_attackers(bb, at, BLACK));
// }

TEST_F(TestMoveGen, TestGenPseudoLegalMoves) {
    Bitboard bb { *Bitboard::from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR") };
    std::vector<EncodedMove> moves;
    MoveGen(moves, bb, at, WHITE, CastlingRights {}, std::nullopt).gen();
    EXPECT_EQ(20, moves.size());
}

TEST_F(TestMoveGen, TestPinnedPieces) {
    Bitboard bb { 
        *Bitboard::from_fen("r1bqkbnr/ppp2ppp/2np4/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R") 
    };
    auto result { pinned_pieces(bb, at, BLACK) };
    EXPECT_EQ(from_square(C6), result) << MaskDisplay(result);

    bb = *Bitboard::from_fen("4k3/3r2r1/8/8/3Q4/8/3K2Nb/8");
    result = pinned_pieces(bb, at, WHITE);
    EXPECT_EQ(from_square(D4), result) << MaskDisplay(result);

    bb = *Bitboard::from_fen("4k3/3q2r1/8/8/3Q4/8/3K2Nb/8");
    result = pinned_pieces(bb, at, WHITE);
    EXPECT_EQ(from_square(D4), result) << MaskDisplay(result);

    bb = *Bitboard::from_fen("2k5/8/4q3/8/8/3R3B/3K4/8");
    result = pinned_pieces(bb, at, BLACK);
    EXPECT_EQ(from_square(E6), result) << MaskDisplay(result);
    
    bb = *Bitboard::from_fen("2k5/8/4q3/2n5/8/2R4B/3K4/8");
    result = pinned_pieces(bb, at, BLACK);
    EXPECT_EQ(mask_from_squares({ C5, E6 }), result) << MaskDisplay(result);
}

TEST_F(TestMoveGen, TestKingInfo) {
    Bitboard bb { *Bitboard::from_fen("8/2kr4/8/8/8/3K4/8/8") };
    auto result = king_danger_squares(bb, at, WHITE);
    EXPECT_EQ(mask_from_squares({
        B8, C8, D8,
        B7, C7, D7, E7, F7, G7, H7,
        B6, C6, D6,
                D5,
                D4,
                D3,
                D2,
                D1
    }), result.king_danger_squares) << MaskDisplay(result.king_danger_squares);
    EXPECT_EQ(from_square(D7), result.king_checking_pieces) << MaskDisplay(result.king_checking_pieces);
    EXPECT_EQ(mask_from_squares({
                D7,
                D6,
                D5,
                D4
    }), result.check_intervention_squares) << MaskDisplay(result.check_intervention_squares);
}