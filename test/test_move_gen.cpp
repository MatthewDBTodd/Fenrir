#include <algorithm>
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

TEST_F(TestMoveGen, TestMoveGen) {
    Bitboard bb { *Bitboard::from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR") };
    CastlingRights castling { *CastlingRights::from_fen("KQkq") };
    std::vector<EncodedMove> moves;
    std::vector<DecodedMove> output;
    MoveGen(moves, bb, at, WHITE, castling, std::nullopt).gen();

    std::size_t expected { 20 };
    EXPECT_EQ(20, moves.size()); // << "Moves: " << output;
    // Hack so I can print out the output if the test fails, not sure why gtest isn't finding
    // the output operator when the test fails, will look into this when I can be bothered
    if (expected != moves.size()) {
        std::transform(moves.begin(), moves.end(), std::back_inserter(output), [](const auto move) {
            return decode(move);
        });
        EXPECT_EQ(output, std::vector<DecodedMove>{});
    }

    moves.clear();
    output.clear();
    bb = *Bitboard::from_fen("8/8/8/3k4/8/3p4/3P4/3K4");
    castling = *CastlingRights::from_fen("-");
    MoveGen(moves, bb, at, WHITE, castling, std::nullopt).gen();
    expected = 2;
    EXPECT_EQ(expected, moves.size());
    if (expected != moves.size()) {
        std::transform(moves.begin(), moves.end(), std::back_inserter(output), [](const auto move) {
            return decode(move);
        });
        EXPECT_EQ(output, std::vector<DecodedMove>{});
    }

    moves.clear();
    output.clear();
    // Test for when en-passant puts oneself in check
    bb = *Bitboard::from_fen("8/8/8/6K1/k2pP2R/8/8/8");
    castling = *CastlingRights::from_fen("-");
    MoveGen(moves, bb, at, BLACK, castling, E3).gen();
    expected = 6;
    EXPECT_EQ(expected, moves.size());
    if (expected != moves.size()) {
        std::transform(moves.begin(), moves.end(), std::back_inserter(output), [](const auto move) {
            return decode(move);
        });
        EXPECT_EQ(output, std::vector<DecodedMove>{});
    }

    moves.clear();
    output.clear();
    bb = *Bitboard::from_fen("4k1r1/8/8/8/8/8/8/R3K2R");
    castling = *CastlingRights::from_fen("KQ");
    MoveGen(moves, bb, at, WHITE, castling, std::nullopt).gen();
    expected = 25;
    EXPECT_EQ(expected, moves.size());
    if (expected != moves.size()) {
        std::transform(moves.begin(), moves.end(), std::back_inserter(output), [](const auto move) {
            return decode(move);
        });
        EXPECT_EQ(output, std::vector<DecodedMove>{});
    }
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
    EXPECT_EQ(mask_from_squares(
        { B8, C8, D8, B7, C7, D7, E7, F7, G7, H7, B6, C6, D6, D5, D4, D3, D2, D1 }
    ), result.king_danger_squares) << MaskDisplay(result.king_danger_squares);
    EXPECT_EQ(from_square(D7), result.king_checking_pieces) << MaskDisplay(result.king_checking_pieces);
    EXPECT_EQ(mask_from_squares({ D7, D6, D5, D4 }), 
              result.check_intervention_squares) << MaskDisplay(result.check_intervention_squares);

    bb = *Bitboard::from_fen("8/8/5k2/8/8/4K3/1Q6/8");
    result = king_danger_squares(bb, at, BLACK);
    EXPECT_EQ(mask_from_squares(
        { A1, B1, C1, A2, C2, D2, E2, F2, G2, H2, A3, B3, C3, D3, F3, B4, D4, E4,
          F4, B5, E5, B6, F6, B7, G7, B8, H8 }
    ), result.king_danger_squares) << MaskDisplay(result.king_danger_squares);
    EXPECT_EQ(from_square(B2), result.king_checking_pieces) << MaskDisplay(result.king_checking_pieces);
    EXPECT_EQ(mask_from_squares({ B2, C3, D4, E5 }), 
              result.check_intervention_squares) << MaskDisplay(result.check_intervention_squares);

    bb = *Bitboard::from_fen("8/8/3p4/1kn5/8/1KP5/8/8");
    result = king_danger_squares(bb, at, WHITE);
    EXPECT_EQ(mask_from_squares(
        { B3, D3, A4, B4, C4, E4, A5, C5, E5, A6, B6, C6, E6, B7, D7 }
    ), result.king_danger_squares) << MaskDisplay(result.king_danger_squares);
    EXPECT_EQ(from_square(C5), result.king_checking_pieces) << MaskDisplay(result.king_checking_pieces);
    EXPECT_EQ(from_square(C5), result.check_intervention_squares) << 
              MaskDisplay(result.check_intervention_squares);

    bb = *Bitboard::from_fen("3k4/1KP5/8/8/8/8/8/8");
    result = king_danger_squares(bb, at, BLACK);
    EXPECT_EQ(mask_from_squares(
        { A6, B6, C6, A7, C7, A8, B8, C8, D8  }
    ), result.king_danger_squares) << MaskDisplay(result.king_danger_squares);
    EXPECT_EQ(from_square(C7), result.king_checking_pieces) << MaskDisplay(result.king_checking_pieces);
    EXPECT_EQ(from_square(C7), result.check_intervention_squares) <<
              MaskDisplay(result.check_intervention_squares);

    bb = *Bitboard::from_fen("8/4r3/8/6k1/2b5/8/4K3/8");
    result = king_danger_squares(bb, at, WHITE);
    EXPECT_EQ(mask_from_squares(
        { E1, F1, A2, E2, B3, D3, E3, E4, F4, G4, H4, B5, D5, E5, F5, H5, A6, E6,
          F6, G6, H6, A7, B7, C7, D7, F7, G7, H7, E8, G8 }
    ), result.king_danger_squares) << MaskDisplay(result.king_danger_squares);
    EXPECT_EQ(mask_from_squares({ C4, E7 }), result.king_checking_pieces) <<
              MaskDisplay(result.king_checking_pieces);
    EXPECT_EQ(mask_from_squares({ C4, E7, D3, E3, E4, E5, E6 }), 
              result.check_intervention_squares) << MaskDisplay(result.check_intervention_squares);
}