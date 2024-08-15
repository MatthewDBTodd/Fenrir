#include <gtest/gtest.h>

#include "castling.h"

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