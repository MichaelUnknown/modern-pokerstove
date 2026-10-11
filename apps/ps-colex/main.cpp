#include "../cli_options.h"
#include <iostream>
#include <map>
#include <set>
#include <peval/Card.h>
#include <peval/CardSet.h>
#include <util/combinations.h>
#include <string>
#include <vector>

using namespace std;
using namespace pokerstove;

#if 0
std::set<CardSet>
expandRankSet(size_t numCards)
{
    combinations cards(52,numCards);
    do
    {
        CardSet hand;
        for (size_t i=0; i<num_cards; i++)
        {
            hand.insert (Card(cards[i]));
        }
        collection.insert(hand.canonize());
        rankHands[hand.rankstr()] = hand.rankColex();
    }
    while (cards.next());
}
#endif

int main(int argc, char** argv)
{
    try
    {
        cxxopts::Options options("ps-colex",
            "Print poker combinations with canonical suits or ranks");
        options.add_options()
            ("help", "produce help message")
            ("n,num-cards", "number of cards in hands", cxxopts::value<size_t>()->default_value("2"))
            ("ranks", "print the set of rank values");
        const auto vm = cli::parse(options, argc, argv, {"num-cards"});
        if (vm.count("help") || argc == 1)
        {
            cout << options.help() << endl;
            return 1;
        }
        size_t num_cards = vm["num-cards"].as<size_t>();
        if (num_cards > 52)
            throw std::invalid_argument("num-cards must be between 0 and 52");

        set<CardSet> canonicalHands;
        map<string, size_t> rankHands;
        combinations cards(52, num_cards);
        do
        {
            CardSet hand;
            for (size_t i = 0; i < num_cards; i++)
            {
                hand.insert(Card(cards[i]));
            }
            canonicalHands.insert(hand.canonize());
            rankHands[hand.rankstr()] = hand.rankColex();
        } while (cards.next());

        if (vm["ranks"].as<bool>())
        {
            for (auto it = rankHands.begin(); it != rankHands.end(); it++)
                cout << it->first << ": " << it->second << '\n';
        }
        else
        {
            for (auto it = canonicalHands.begin(); it != canonicalHands.end();
                 it++)
                cout << it->str() << ": " << it->colex() << '\n';
        }
    }
    catch (std::exception& e)
    {
        cerr << "-- caught exception--\n" << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        cerr << "Exception of unknown type!\n";
        return 1;
    }
    return 0;
}
