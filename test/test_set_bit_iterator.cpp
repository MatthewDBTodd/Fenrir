#include <gtest/gtest.h>

#include "set_bit_iterator.h"

#include <vector>

struct TestCase {
    const std::uint64_t val;
    const std::vector<std::uint64_t> expected_permutations;
};

TEST(TestSetBits, TestSetBits) {
    const std::vector<TestCase> test_cases {
        {
            255,
            { 1, 2, 4, 8, 16, 32, 64, 128 }
        },
        { 
            65535,
            { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 }
        },
        {
            128,
            { 128 }
        },
        { 
            65, 
            { 1, 64 }
        },
        {
            43690,
            { 2, 8, 32, 128, 512, 2048, 8192, 32768 }
        }
    };
    for (const auto &test_case : test_cases) {
        const std::vector<std::uint64_t> result(SetBits(test_case.val).begin(),
                                                SetBits(test_case.val).end());
        EXPECT_EQ(test_case.expected_permutations, result);
    }
}