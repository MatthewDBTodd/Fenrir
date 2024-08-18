#include <gtest/gtest.h>

#include "board.h"

#include <string_view>

// Piece placement section of fen are tested elsewhere 
TEST(TestBoard, TestBoardFromFen) {
    std::string_view fen { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
    std::optional<Board> board { Board::from_fen(fen) };
    ASSERT_TRUE(board.has_value());

    EXPECT_EQ(1, board->fullmove_count);
    EXPECT_EQ(0, board->quiet_half_moves);
    EXPECT_EQ(WHITE, board->turn_colour);
    EXPECT_TRUE(board->castling.can_castle(WHITE, KING));
    EXPECT_TRUE(board->castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(board->castling.can_castle(BLACK, KING));
    EXPECT_TRUE(board->castling.can_castle(BLACK, QUEEN));
    EXPECT_FALSE(board->en_passant.has_value());

    fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    board = Board::from_fen(fen);
    ASSERT_TRUE(board.has_value());

    EXPECT_EQ(1, board->fullmove_count);
    EXPECT_EQ(0, board->quiet_half_moves);
    EXPECT_EQ(BLACK, board->turn_colour);
    EXPECT_TRUE(board->castling.can_castle(WHITE, KING));
    EXPECT_TRUE(board->castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(board->castling.can_castle(BLACK, KING));
    EXPECT_TRUE(board->castling.can_castle(BLACK, QUEEN));
    ASSERT_TRUE(board->en_passant.has_value());
    EXPECT_EQ(E3, *board->en_passant);

    fen = "r1bqkb1r/pppp1ppp/2n2n2/1B2p3/4P3/2N2N2/PPPP1PPP/R1BQK2R b KQkq - 5 4";
    board = Board::from_fen(fen);
    ASSERT_TRUE(board.has_value());

    EXPECT_EQ(4, board->fullmove_count);
    EXPECT_EQ(5, board->quiet_half_moves);
    EXPECT_EQ(BLACK, board->turn_colour);
    EXPECT_TRUE(board->castling.can_castle(WHITE, KING));
    EXPECT_TRUE(board->castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(board->castling.can_castle(BLACK, KING));
    EXPECT_TRUE(board->castling.can_castle(BLACK, QUEEN));
    EXPECT_FALSE(board->en_passant.has_value());

    fen = "r1bqk2r/pppp1ppp/2n2n2/1B2p3/1b2P3/2N2N2/PPPP1PPP/R1BQ1RK1 b kq - 7 5";
    board = Board::from_fen(fen);
    ASSERT_TRUE(board.has_value());

    EXPECT_EQ(5, board->fullmove_count);
    EXPECT_EQ(7, board->quiet_half_moves);
    EXPECT_EQ(BLACK, board->turn_colour);
    EXPECT_FALSE(board->castling.can_castle(WHITE, KING));
    EXPECT_FALSE(board->castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(board->castling.can_castle(BLACK, KING));
    EXPECT_TRUE(board->castling.can_castle(BLACK, QUEEN));
    EXPECT_FALSE(board->en_passant.has_value());

    fen = "r1bq1rk1/pppp1ppp/2n2n2/1B2p3/1b2P3/2N2N2/PPPP1PPP/R1BQ1RK1 w - - 8 6";
    board = Board::from_fen(fen);
    ASSERT_TRUE(board.has_value());

    EXPECT_EQ(6, board->fullmove_count);
    EXPECT_EQ(8, board->quiet_half_moves);
    EXPECT_EQ(WHITE, board->turn_colour);
    EXPECT_FALSE(board->castling.can_castle(WHITE, KING));
    EXPECT_FALSE(board->castling.can_castle(WHITE, QUEEN));
    EXPECT_FALSE(board->castling.can_castle(BLACK, KING));
    EXPECT_FALSE(board->castling.can_castle(BLACK, QUEEN));
    EXPECT_FALSE(board->en_passant.has_value());

    // One section missing
    fen = "r1bq1rk1/pppp1ppp/2n2n2/1B2p3/1b2P3/2N2N2/PPPP1PPP/R1BQ1RK1 w - 8 6";
    board = Board::from_fen(fen);
    EXPECT_FALSE(board.has_value());

    // invalid piece placement section
    fen = "rnbqkbnr/pppppppp/8/8/8/8/yPPPPPPP/RNBQKBNR w KQkq - 0 1";
    board = Board::from_fen(fen);
    EXPECT_FALSE(board.has_value());

    // invalid turn colour
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR - KQkq - 0 1";
    board = Board::from_fen(fen);
    EXPECT_FALSE(board.has_value());

    // invalid castling rights
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkqK - 0 1";
    board = Board::from_fen(fen);
    EXPECT_FALSE(board.has_value());

    // invalid en-passant square
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq K3 0 1";
    board = Board::from_fen(fen);
    EXPECT_FALSE(board.has_value());

    // non int half move count
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - x 1";
    board = Board::from_fen(fen);
    EXPECT_FALSE(board.has_value());

    // out of range half move count
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 256 1";
    board = Board::from_fen(fen);
    EXPECT_FALSE(board.has_value());

    // out of range total move count
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 -1";
    board = Board::from_fen(fen);
    EXPECT_FALSE(board.has_value());

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 65536";
    board = Board::from_fen(fen);
    EXPECT_FALSE(board.has_value());
}
