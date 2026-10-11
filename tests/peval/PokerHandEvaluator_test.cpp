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

TEST(EquityResult, precision_and_spacing)
{
    pokerstove::EquityResult result;
    EXPECT_EQ(result.str(), "0 0 0 0");
    result.winShares = 1.0 / 3.0;
    result.tieShares = 1e-9;
    result.equity = -0.0;
    result.equity2 = 1e20;
    EXPECT_EQ(result.str(), "0.33333333333333331 1.0000000000000001e-09 -0 1e+20");
}

TEST(PokerHandEvaluator, case_insensitive_factory_and_invalid_identifiers)
{
    for (const auto game : {"H", "Holdem", "OMAHA", "OMaHa/8", "R", "D"})
        EXPECT_NE(pokerstove::PokerHandEvaluator::alloc(game), nullptr) << game;
    EXPECT_EQ(pokerstove::PokerHandEvaluator::alloc(""), nullptr);
    EXPECT_EQ(pokerstove::PokerHandEvaluator::alloc("invalid"), nullptr);
    EXPECT_EQ(pokerstove::PokerHandEvaluator::alloc("o-unsupported"), nullptr);
}
