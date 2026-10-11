#include <util/combinations.h>
#include <gtest/gtest.h>
#include <limits>
#include <vector>

using namespace pokerstove;

TEST(Combinations, holdem)
{
    // if we run though all the 2 card combos we get 52c2 hands
    int count = 0;
    combinations cards(52, 2);
    do {
        count += 1;
    } while (cards.next());

    EXPECT_EQ(count, 1326);
}

TEST(Combinations, exact_counts)
{
    EXPECT_EQ(choose(0, 0), 1);
    EXPECT_EQ(choose(0, 1), 0);
    EXPECT_EQ(choose(4, 8), 0);
    EXPECT_EQ(choose(52, 0), 1);
    EXPECT_EQ(choose(52, 52), 1);
    EXPECT_EQ(choose(52, 2), 1326);
    EXPECT_EQ(choose(52, 5), 2598960);
    EXPECT_EQ(choose(52, 7), 133784560);
    EXPECT_EQ(choose(52, 26), UINT64_C(495918532948104));
    // Above double's exact integer range; no floating-point round trip.
    EXPECT_EQ(choose(67, 33), UINT64_C(14226520737620288370));
    EXPECT_EQ(choose(67, 34), UINT64_C(14226520737620288370));
}

TEST(Combinations, matches_integer_pascal_triangle)
{
    // Independent additive reference covers all coefficients through row 67.
    std::vector<std::uint64_t> row{1};
    for (int n = 0; n <= 67; ++n)
    {
        for (int k = 0; k <= n; ++k)
            EXPECT_EQ(choose(n, k), row[k]) << n << ", " << k;
        row.push_back(1);
        if (n < 67)
            for (int k = n; k > 0; --k)
                row[k] += row[k - 1];
    }
}

TEST(Combinations, invalid_arguments_and_overflow)
{
    EXPECT_THROW(choose(-1, 0), std::invalid_argument);
    EXPECT_THROW(choose(4, -1), std::invalid_argument);
    EXPECT_THROW(choose(-1, -1), std::invalid_argument);
    EXPECT_THROW(choose(68, 34), std::overflow_error);
    EXPECT_THROW(choose(100, 50), std::overflow_error);
    EXPECT_EQ(choose(std::numeric_limits<int>::max(), 0), 1);
    EXPECT_EQ(choose(std::numeric_limits<int>::max(),
                     std::numeric_limits<int>::max()), 1);
    EXPECT_EQ(choose(std::numeric_limits<int>::max(), 2),
              UINT64_C(2305843005992468481));
    EXPECT_THROW(choose(std::numeric_limits<int>::max(), 3), std::overflow_error);
}

TEST(Combinations, string_format)
{
    combinations cards(12, 2);
    EXPECT_EQ(cards.str(), "0 1 ");
    while (cards.next()) {}
    EXPECT_EQ(cards.str(), "10 11 ");
    EXPECT_EQ(combinations(52, 0).str(), "");
}
