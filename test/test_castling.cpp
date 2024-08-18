#include <gtest/gtest.h>

#include "castling.h"
#include "decoded_move.h"

TEST(TestCastling, TestFromFen) {
    std::string_view castling_fen { "-" };
    auto castling_rights { CastlingRights::from_fen(castling_fen) };
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_FALSE(castling_rights->can_castle(WHITE, KING));
    EXPECT_FALSE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, KING));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "q";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_FALSE(castling_rights->can_castle(WHITE, KING));
    EXPECT_FALSE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, KING));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "k";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_FALSE(castling_rights->can_castle(WHITE, KING));
    EXPECT_FALSE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, KING));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "kq";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_FALSE(castling_rights->can_castle(WHITE, KING));
    EXPECT_FALSE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, KING));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "Q";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_FALSE(castling_rights->can_castle(WHITE, KING));
    EXPECT_TRUE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, KING));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "Qq";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_FALSE(castling_rights->can_castle(WHITE, KING));
    EXPECT_TRUE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, KING));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "Qk";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_FALSE(castling_rights->can_castle(WHITE, KING));
    EXPECT_TRUE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, KING));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "Qkq";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_FALSE(castling_rights->can_castle(WHITE, KING));
    EXPECT_TRUE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, KING));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "K";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_TRUE(castling_rights->can_castle(WHITE, KING));
    EXPECT_FALSE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, KING));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "Kq";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_TRUE(castling_rights->can_castle(WHITE, KING));
    EXPECT_FALSE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, KING));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "Kk";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_TRUE(castling_rights->can_castle(WHITE, KING));
    EXPECT_FALSE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, KING));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "Kkq";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_TRUE(castling_rights->can_castle(WHITE, KING));
    EXPECT_FALSE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, KING));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "KQ";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_TRUE(castling_rights->can_castle(WHITE, KING));
    EXPECT_TRUE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, KING));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "KQq";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_TRUE(castling_rights->can_castle(WHITE, KING));
    EXPECT_TRUE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, KING));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "KQk";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_TRUE(castling_rights->can_castle(WHITE, KING));
    EXPECT_TRUE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, KING));
    EXPECT_FALSE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "KQkq";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_TRUE(castling_rights.has_value());
    EXPECT_TRUE(castling_rights->can_castle(WHITE, KING));
    EXPECT_TRUE(castling_rights->can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, KING));
    EXPECT_TRUE(castling_rights->can_castle(BLACK, QUEEN));

    castling_fen = "Kjkq";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_FALSE(castling_rights.has_value());

    castling_fen = "KQkqK";
    castling_rights = CastlingRights::from_fen(castling_fen);
    ASSERT_FALSE(castling_rights.has_value());
}

TEST(TestCastling, TestUpdateCastlingRights) {
    CastlingRights castling {};
    EXPECT_TRUE(castling.can_castle(WHITE, KING));
    EXPECT_TRUE(castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling.can_castle(BLACK, KING));
    EXPECT_TRUE(castling.can_castle(BLACK, QUEEN));

    DecodedMove move {
        move_type_v::Quiet { move_type_v::Common {
            E1, E2, KING, WHITE
        }}
    };
    castling.update_castling(move);
    EXPECT_FALSE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling.can_castle(BLACK, KING));
    EXPECT_TRUE(castling.can_castle(BLACK, QUEEN));

    move = move_type_v::Quiet { move_type_v::Common {
        E8, D8, KING, BLACK
    }};
    castling.update_castling(move);
    EXPECT_FALSE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling.can_castle(BLACK, KING));
    EXPECT_FALSE(castling.can_castle(BLACK, QUEEN));

    castling = CastlingRights {};
    move = move_type_v::Quiet { move_type_v::Common {
        A1, A5, ROOK, WHITE
    }};
    castling.update_castling(move);
    EXPECT_TRUE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling.can_castle(BLACK, KING));
    EXPECT_TRUE(castling.can_castle(BLACK, QUEEN));

    move = move_type_v::Quiet { move_type_v::Common {
        H1, H8, ROOK, WHITE
    }};
    castling.update_castling(move);
    EXPECT_FALSE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling.can_castle(BLACK, KING));
    EXPECT_TRUE(castling.can_castle(BLACK, QUEEN));

    move = move_type_v::Quiet { move_type_v::Common {
        A8, A6, ROOK, BLACK
    }};
    castling.update_castling(move);
    EXPECT_FALSE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling.can_castle(BLACK, KING));
    EXPECT_FALSE(castling.can_castle(BLACK, QUEEN));

    move = move_type_v::Quiet { move_type_v::Common {
        H8, H6, ROOK, BLACK
    }};
    castling.update_castling(move);
    EXPECT_FALSE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling.can_castle(BLACK, KING));
    EXPECT_FALSE(castling.can_castle(BLACK, QUEEN));

    castling = CastlingRights {};
    move = move_type_v::Capture { move_type_v::Common {
        B2, A1, PAWN, BLACK
    }, 
    ROOK };
    castling.update_castling(move);
    EXPECT_TRUE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling.can_castle(BLACK, KING));
    EXPECT_TRUE(castling.can_castle(BLACK, QUEEN));

    move = move_type_v::Capture { move_type_v::Common {
        D5, H1, BISHOP, BLACK
    },
    ROOK };
    castling.update_castling(move);
    EXPECT_FALSE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling.can_castle(BLACK, KING));
    EXPECT_TRUE(castling.can_castle(BLACK, QUEEN));

    move = move_type_v::Capture { move_type_v::Common {
        B6, A8, KNIGHT, WHITE
    },
    ROOK };
    castling.update_castling(move);
    EXPECT_FALSE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling.can_castle(BLACK, KING));
    EXPECT_FALSE(castling.can_castle(BLACK, QUEEN));

    move = move_type_v::Capture { move_type_v::Common {
        H1, H8, QUEEN, WHITE
    }, 
    ROOK };
    castling.update_castling(move);
    EXPECT_FALSE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling.can_castle(BLACK, KING));
    EXPECT_FALSE(castling.can_castle(BLACK, QUEEN));

    castling = CastlingRights {};
    move = move_type_v::CastleKingSide { move_type_v::Common {
        E1, G1, KING, WHITE
    }};
    castling.update_castling(move);
    EXPECT_FALSE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_TRUE(castling.can_castle(BLACK, KING));
    EXPECT_TRUE(castling.can_castle(BLACK, QUEEN));

    move = move_type_v::CastleQueenSide { move_type_v::Common {
        E8, C8, KING, BLACK
    }};
    castling.update_castling(move);
    EXPECT_FALSE(castling.can_castle(WHITE, KING));
    EXPECT_FALSE(castling.can_castle(WHITE, QUEEN));
    EXPECT_FALSE(castling.can_castle(BLACK, KING));
    EXPECT_FALSE(castling.can_castle(BLACK, QUEEN));
}