#include <gtest/gtest.h>

#include "utility.h"

TEST(TestUtility, TestStringSplit) {
    const std::string s { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
    const std::vector<std::string_view> expected {
        "rnbqkbnr", "pppppppp", "8", "8", "8", "8", "PPPPPPPP", "RNBQKBNR w KQkq - 0 1"
    };
    const auto actual { utility::split(s, '/') };
    EXPECT_EQ(expected, actual);
}

TEST(TestUtility, TestStringSplitEmptyString) {
    const std::string s {};
    EXPECT_EQ(std::vector<std::string_view>(), utility::split(s, '/'));
}

TEST(TestUtility, TestStringSplitNoMatches) {
    const std::string s { "This is a test string with no slashes" };
    const std::vector<std::string_view> expected { s };
    const auto actual { utility::split(s, '/') };
    EXPECT_EQ(expected, actual);
}

TEST(TestUtility, TestStringOnlyMatches) {
    const std::string s { "/////////" };
    EXPECT_EQ(std::vector<std::string_view>(), utility::split(s, '/'));
}