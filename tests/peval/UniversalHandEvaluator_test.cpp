#include <peval/UniversalHandEvaluator.h>
#include <gtest/gtest.h>
#include <iostream>

using namespace pokerstove;
using namespace std;

TEST(UniversalHandEvaluator, Construct)
{
    UniversalHandEvaluator eval(4,4,3,5,2,&CardSet::evaluateHigh, NULL);
    EXPECT_EQ(true, eval.usesSuits());
    EXPECT_EQ(5, eval.boardSize());
}

TEST(UniversalHandEvaluator, FiveCardOmaha)
{
    UniversalHandEvaluator oeval(5,5,3,5,2,&CardSet::evaluateHigh, NULL);
    CardSet hand("2d3dKhQd8h");
    CardSet board("2c3c4c");
    PokerEvaluation eval = oeval.eval(hand, board);
    EXPECT_EQ(TWO_PAIR, eval.type());
    EXPECT_EQ(Rank("3"), eval.majorRank());
    EXPECT_EQ(Rank("2"), eval.minorRank());
}

TEST(UniversalHandEvaluator, SixCardOmaha)
{
    UniversalHandEvaluator oeval(6,6,3,5,2,&CardSet::evaluateHigh, NULL);
    CardSet hand("2d3dKhQd8h8d");
    CardSet board("2c3c4c");
    PokerEvaluation eval = oeval.eval(hand, board);
    EXPECT_EQ(TWO_PAIR, eval.type());
    EXPECT_EQ(Rank("3"), eval.majorRank());
    EXPECT_EQ(Rank("2"), eval.minorRank());
}

TEST(UniversalHandEvaluator, invalid_card_count_messages)
{
    UniversalHandEvaluator evaluator(4, 4, 3, 5, 2, &CardSet::evaluateHigh, nullptr);
    try
    {
        evaluator.evaluateHand(CardSet("AcAs"), CardSet());
        FAIL() << "invalid pocket accepted";
    }
    catch (const std::invalid_argument& error)
    {
        EXPECT_STREQ(error.what(), "UnivHandEval: 2: invalid number of pocket cards");
    }
    try
    {
        evaluator.evaluateHand(CardSet("AcAsKdKh"), CardSet("2c"));
        FAIL() << "invalid board accepted";
    }
    catch (const std::invalid_argument& error)
    {
        EXPECT_STREQ(error.what(), "UnivHandEval: 1 unsupported number of board cards");
    }
}
