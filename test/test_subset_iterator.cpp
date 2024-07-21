#include <gtest/gtest.h>

#include "subset_iterator.h"

#include <vector>
#include <numeric>

using namespace utility;

struct TestCase {
    const std::uint64_t val;
    const std::vector<std::uint64_t> expected_permutations;
};

std::vector<std::uint64_t> range_to_vec(const std::uint64_t min, const std::uint64_t max) {
    std::vector<std::uint64_t> rv(max-min);
    std::iota(rv.begin(), rv.end(), min);
    return rv;
}

TEST(TestSubsetIterator, TestSubsetIterator) {
    const std::vector<TestCase> test_cases {
        { 
            341, 
            { 0, 1, 4, 5, 16, 17, 20, 21, 64, 65, 68, 69, 80, 81, 84, 85, 256, 257, 
            260, 261, 272, 273, 276, 277, 320, 321, 324, 325, 336, 337, 340, 341 }
        },
        { 0, { 0 } },
        { 1, { 0, 1 } },
        { 255, range_to_vec(0, 256) },
        { (1ull << 63), { 0, (1ull << 63) } }
    };
    for (const auto &test_case : test_cases) {
        const std::vector<std::uint64_t> result(SubsetIterator(test_case.val).begin(),
                                                SubsetIterator(test_case.val).end());
        EXPECT_EQ(test_case.expected_permutations, result);
    }
}