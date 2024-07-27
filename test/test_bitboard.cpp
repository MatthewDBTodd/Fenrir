#include <gtest/gtest.h>

#include "bitboard.h"
#include "test_helpers.h"

#include <string_view>

// Bitboard only gets passed the pieces section of the FEN, so not testing full FENs
TEST(TestBitboard, TestBitboardFromStartingFen) {
    std::string_view pieces_fen { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR" };
    std::optional<Bitboard> ob { Bitboard::from_fen(pieces_fen) };
    ASSERT_TRUE(ob.has_value());
    const Bitboard bb { std::move(*ob) };
    EXPECT_EQ(
        mask_from_squares(
            { A1, B1, C1, D1, E1, F1, G1, H1, A2, B2, C2, D2, E2, F2, G2, H2,
              A7, B7, C7, D7, E7, F7, G7, H7, A8, B8, C8, D8, E8, F8, G8, H8 }
        ), 
        bb.entire_mask()) << "result: " << bb;

    EXPECT_EQ(
        mask_from_squares(
            { A1, B1, C1, D1, E1, F1, G1, H1, A2, B2, C2, D2, E2, F2, G2, H2 }
        ),
        bb.colour_mask(WHITE));

    EXPECT_EQ(
        mask_from_squares(
            { A7, B7, C7, D7, E7, F7, G7, H7, A8, B8, C8, D8, E8, F8, G8, H8 }
        ),
        bb.colour_mask(BLACK));
    
    EXPECT_EQ(
        mask_from_squares(
            { A2, B2, C2, D2, E2, F2, G2, H2, A7, B7, C7, D7, E7, F7, G7, H7 }
        ),
        bb.piece_mask(PAWN));
    
    EXPECT_EQ(mask_from_squares({ A2, B2, C2, D2, E2, F2, G2, H2 }), 
              bb.colour_piece_mask(WHITE, PAWN));
    
    EXPECT_EQ(mask_from_squares({ A7, B7, C7, D7, E7, F7, G7, H7 }),
              bb.colour_piece_mask(BLACK, PAWN));
    
    EXPECT_EQ(mask_from_squares({ E1, E8 }), bb.piece_mask(KING));
    EXPECT_EQ(mask_from_squares({ E1 }), bb.colour_piece_mask(WHITE, KING));
    EXPECT_EQ(mask_from_squares({ E8 }), bb.colour_piece_mask(BLACK, KING));


    EXPECT_EQ(mask_from_squares({ D1, D8 }), bb.piece_mask(QUEEN));
    EXPECT_EQ(mask_from_squares({ D1 }), bb.colour_piece_mask(WHITE, QUEEN));
    EXPECT_EQ(mask_from_squares({ D8 }), bb.colour_piece_mask(BLACK, QUEEN));

    EXPECT_EQ(mask_from_squares({ C1, F1, C8, F8 }), bb.piece_mask(BISHOP));
    EXPECT_EQ(mask_from_squares({ C1, F1 }), bb.colour_piece_mask(WHITE, BISHOP));
    EXPECT_EQ(mask_from_squares({ C8, F8 }), bb.colour_piece_mask(BLACK, BISHOP));

    EXPECT_EQ(mask_from_squares({ B1, G1, B8, G8 }), bb.piece_mask(KNIGHT));
    EXPECT_EQ(mask_from_squares({ B1, G1 }), bb.colour_piece_mask(WHITE, KNIGHT));
    EXPECT_EQ(mask_from_squares({ B8, G8 }), bb.colour_piece_mask(BLACK, KNIGHT));

    EXPECT_EQ(mask_from_squares({ A1, H1, A8, H8 }), bb.piece_mask(ROOK));
    EXPECT_EQ(mask_from_squares({ A1, H1 }), bb.colour_piece_mask(WHITE, ROOK));
    EXPECT_EQ(mask_from_squares({ A8, H8 }), bb.colour_piece_mask(BLACK, ROOK));
}

TEST(TestBitboard, TestBitboardFromFen) {
    std::string_view pieces_fen { "r2q1rk1/2p2ppp/p1np1n2/1pbNp1Bb/4P3/1B1P1N1P/PPP2PP1/R2Q1RK1" };
    std::optional<Bitboard> ob { Bitboard::from_fen(pieces_fen) };
    ASSERT_TRUE(ob.has_value());
    const Bitboard bb { std::move(*ob) };

    EXPECT_EQ(
        mask_from_squares(
            { A1, D1, F1, G1, A2, B2, C2, F2, G2, B3, D3, F3, H3, E4,
              B5, C5, D5, E5, G5, H5, A6, C6, D6, F6, C7, F7, G7, H7,
              A8, D8, F8, G8 }),
        bb.entire_mask());
    
    EXPECT_EQ(
        mask_from_squares(
            { A1, D1, F1, G1, A2, B2, C2, F2, G2, B3, D3, F3, H3, E4, D5, G5 }),
        bb.colour_mask(WHITE));
    
    EXPECT_EQ(
        mask_from_squares(
            { B5, C5, E5, H5, A6, C6, D6, F6, C7, F7, G7, H7, A8, D8, F8, G8 }),
        bb.colour_mask(BLACK));

    EXPECT_EQ(
        mask_from_squares(
            { A2, B2, C2, D3, E4, F2, G2, H3, A6, B5, C7, D6, E5, F7, G7, H7 }),
        bb.piece_mask(PAWN));
    
    EXPECT_EQ(mask_from_squares({ A2, B2, C2, D3, E4, F2, G2, H3 }), 
              bb.colour_piece_mask(WHITE, PAWN));
    
    EXPECT_EQ(mask_from_squares({ A6, B5, C7, D6, E5, F7, G7, H7 }),
              bb.colour_piece_mask(BLACK, PAWN));
    
    EXPECT_EQ(mask_from_squares({ G1, G8 }), bb.piece_mask(KING));
    EXPECT_EQ(mask_from_squares({ G1 }), bb.colour_piece_mask(WHITE, KING));
    EXPECT_EQ(mask_from_squares({ G8 }), bb.colour_piece_mask(BLACK, KING));


    EXPECT_EQ(mask_from_squares({ D1, D8 }), bb.piece_mask(QUEEN));
    EXPECT_EQ(mask_from_squares({ D1 }), bb.colour_piece_mask(WHITE, QUEEN));
    EXPECT_EQ(mask_from_squares({ D8 }), bb.colour_piece_mask(BLACK, QUEEN));

    EXPECT_EQ(mask_from_squares({ B3, G5, C5, H5 }), bb.piece_mask(BISHOP));
    EXPECT_EQ(mask_from_squares({ B3, G5 }), bb.colour_piece_mask(WHITE, BISHOP));
    EXPECT_EQ(mask_from_squares({ C5, H5 }), bb.colour_piece_mask(BLACK, BISHOP));

    EXPECT_EQ(mask_from_squares({ D5, F3, C6, F6 }), bb.piece_mask(KNIGHT));
    EXPECT_EQ(mask_from_squares({ D5, F3 }), bb.colour_piece_mask(WHITE, KNIGHT));
    EXPECT_EQ(mask_from_squares({ C6, F6 }), bb.colour_piece_mask(BLACK, KNIGHT));

    EXPECT_EQ(mask_from_squares({ A1, F1, A8, F8 }), bb.piece_mask(ROOK));
    EXPECT_EQ(mask_from_squares({ A1, F1 }), bb.colour_piece_mask(WHITE, ROOK));
    EXPECT_EQ(mask_from_squares({ A8, F8 }), bb.colour_piece_mask(BLACK, ROOK));
}

TEST(TestBitboard, TestBitboard) {
    Bitboard bb;
    bb.place_unchecked(BLACK, ROOK, E4);
    EXPECT_EQ(mask_from_squares({ E4 }), bb.entire_mask());
    EXPECT_EQ(mask_from_squares({ E4 }), bb.colour_mask(BLACK));
    EXPECT_EQ(mask_from_squares({ E4 }), bb.piece_mask(ROOK));
    EXPECT_EQ(mask_from_squares({ E4 }), bb.colour_piece_mask(BLACK, ROOK));
    EXPECT_EQ('r', bb.square_occupant(E4));
    EXPECT_EQ(' ', bb.square_occupant(D4));
    EXPECT_TRUE(bb.square_empty(D4));
    EXPECT_FALSE(bb.square_empty(E4));

    bb.remove_unchecked(BLACK, ROOK, E4);
    EXPECT_EQ(Bitboard(), bb);
    EXPECT_EQ(' ', bb.square_occupant(E4));
    EXPECT_EQ(' ', bb.square_occupant(D4));
    EXPECT_TRUE(bb.square_empty(D4));
    EXPECT_TRUE(bb.square_empty(E4));
}
