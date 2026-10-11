#include <peval/CardSet.h>
#include <penum/ShowdownEnumerator.h>

#include <vector>

int main()
{
    using namespace pokerstove;
    const auto evaluator = PokerHandEvaluator::alloc("h");
    const std::vector<CardDistribution> hands{
        CardDistribution(CardSet("AcAs")), CardDistribution(CardSet("KhKd"))};
    const auto results = ShowdownEnumerator().calculateEquity(
        hands, CardSet("2c3d4h5s9c"), evaluator);
    return results.size() == 2 && results[0].winShares == 1.0 &&
                   results[1].winShares == 0.0
               ? 0
               : 1;
}
