#include <peval/PokerEvaluation.h>
#include <peval/CardSet.h>
#include <gtest/gtest.h>

using namespace pokerstove;


TEST(PokerEvaluation, High)
{
    CardSet cards("AcAs8c8hKd");    // dead man's hand

    PokerEvaluation eval = cards.evaluateHigh();
    EXPECT_EQ(eval.type(), TWO_PAIR);
    EXPECT_EQ(eval.majorRank(), Rank::Ace());
    EXPECT_EQ(eval.minorRank(), Rank::Eight());
    EXPECT_EQ(true, true);
}

TEST(PokerEvaluation, preserved_text_and_padding)
{
    const auto pair = CardSet("AcAsKdQhJc").evaluateHigh();
    EXPECT_EQ(pair.str(), "one pair:      AKQJ ");
    EXPECT_EQ(pair.toStringPretty(), "One pair: Aces, KQJ kickers");
    const auto straight = CardSet("2c3d4h5s6c").evaluateHigh();
    EXPECT_EQ(straight.str(), "straight:      6    ");
    EXPECT_EQ(straight.toStringPretty(), "Straight:  to the Six");
    const auto full_house = CardSet("AcAsAdKhKd").evaluateHigh();
    EXPECT_EQ(full_house.str(), "full house:    AK   ");
    EXPECT_EQ(full_house.toStringPretty(), "Full house: Aces over Kings");
}
