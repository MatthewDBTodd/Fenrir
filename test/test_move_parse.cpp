#include <gtest/gtest.h>

#include "board.h"
#include "move_parse.h"

TEST(TestMoveParse, TestMoveParse) {
    Board b { *Board::init() };
    DecodedMove expected {
        move_type_v::Quiet { move_type_v::Common {
            G1, F3, KNIGHT, WHITE
        }}
    };
    auto result { parse_move_input("g1f3", b) };
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(expected, result);

    expected = move_type_v::DoublePawnPush { move_type_v::Common {
        E2, E4, PAWN, WHITE
    }, E3 };
    result = parse_move_input("e2e4", b);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(expected, result);

    b = *Board::init("rnbqkb1r/pppp1ppp/5n2/4N3/4P3/8/PPPP1PPP/RNBQKB1R b KQkq - 0 3");
    expected = move_type_v::Capture { move_type_v::Common {
        F6, E4, KNIGHT, BLACK
    }, PAWN };
    result = parse_move_input("f6e4", b);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(expected, result);

    b = *Board::init("r1bqkb1r/pppp1ppp/2n2n2/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4");
    expected = move_type_v::CastleKingSide { move_type_v::Common {
        E1, G1, KING, WHITE
    }};
    result = parse_move_input("e1g1", b);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(expected, result);

    b = *Board::init("r3kb1r/ppp1qppp/2np1n2/1B2p1B1/4P1b1/2NP1N2/PPP2PPP/R2Q1RK1 b kq - 2 7");
    expected = move_type_v::CastleQueenSide { move_type_v::Common {
        E8, C8, KING, BLACK
    }};
    result = parse_move_input("e8c8", b);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(expected, result);

    b = *Board::init("rnbqkbnr/p1ppp1pp/1p6/4Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
    expected = move_type_v::EnPassant { move_type_v::Common {
        E5, F6, PAWN, WHITE
    }, F5 };
    result = parse_move_input("e5f6", b);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(expected, result);

    b = *Board::init("rnbqkbnr/ppp1pppp/8/8/8/2N2NP1/Pp1PPP1P/R1BQKB1R b KQkq - 0 5");
    expected = move_type_v::MovePromotion { move_type_v::Common {
        B2, B1, PAWN, BLACK
    }, QUEEN };
    result = parse_move_input("b2b1Q", b);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(expected, result);

    b = *Board::init("rn1qkb1r/pP2pppp/5n2/5b2/8/8/PPPP1PPP/RNBQKBNR w KQkq - 1 5");
    expected = move_type_v::CapturePromotion { move_type_v::Common {
        B7, A8, PAWN, WHITE
    }, ROOK, QUEEN };
    result = parse_move_input("b7a8Q", b);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(expected, result);
}