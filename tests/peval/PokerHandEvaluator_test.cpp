#include <peval/PokerHandEvaluator.h>
#include <gtest/gtest.h>

TEST(PokerHandEvaluator, OmahaHigh)
{
    using namespace pokerstove;

    std::shared_ptr<PokerHandEvaluator> evaluator = PokerHandEvaluator::alloc("O");
    EXPECT_EQ(true, evaluator->usesSuits());
    EXPECT_EQ(5, evaluator->boardSize());
}

TEST(PokerHandEvaluator, InvalidGame)
{
    using namespace pokerstove;

    std::shared_ptr<PokerHandEvaluator> evaluator = PokerHandEvaluator::alloc("x");
    EXPECT_EQ(nullptr, evaluator);
}


TEST(PokerHandEvaluator, SupportedFactoryNames)
{
    using namespace pokerstove;
    for (const char* game : {"h", "k", "l", "3", "o", "o8", "o/8", "omaha",
                             "omaha/8", "o5", "o6", "o5/8", "o6/8", "r", "s",
                             "q", "d", "t", "T", "e", "b"})
    {
        const auto evaluator = PokerHandEvaluator::alloc(game);
        ASSERT_NE(nullptr, evaluator) << game;
        EXPECT_GT(evaluator->handSize(), 0) << game;
    }
}
