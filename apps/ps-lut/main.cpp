#include <iomanip>
#include "../cli_options.h"
#include <iostream>
#include <peval/Card.h>
#include <peval/CardSet.h>
#include <peval/CardSetGenerators.h>
#include <peval/PokerHandEvaluator.h>
#include <util/combinations.h>
#include <string>
#include <vector>

using namespace std;
using namespace pokerstove;

int main(int argc, char** argv)
{
    try
    {
        cxxopts::Options options("ps-lut", "Create a lookup table for poker hands");
        options.add_options()
            ("help", "produce help message")
            ("p,pocket-count", "number of pocket cards to use", cxxopts::value<size_t>()->default_value("2"))
            ("b,board-count", "number of board cards to use", cxxopts::value<size_t>()->default_value("3"))
            ("g,game", "game to use for evaluation", cxxopts::value<string>()->default_value("O"))
            ("ranks", "print the set of rank values");
        const auto vm = cli::parse(options, argc, argv, {"pocket-count", "board-count", "game"});
        if (vm.count("help") || argc == 1)
        {
            cout << options.help() << endl;
            return 1;
        }
        size_t pocketCount = vm["pocket-count"].as<size_t>();
        size_t boardCount = vm["board-count"].as<size_t>();
        if (pocketCount > 52 || boardCount > 52)
            throw std::invalid_argument("card counts must be between 0 and 52");
        string game = vm["game"].as<string>();
        auto evaluator = PokerHandEvaluator::alloc(game);
        if (!evaluator)
            throw std::invalid_argument("Unsupported game: " + game);
        bool ranks = vm["ranks"].as<bool>();
        // make the sets
        Card::Grouping grouping = Card::SUIT_CANONICAL;
        if (ranks)
            grouping = Card::RANK;
        set<CardSet> pockets = createCardSet(pocketCount, grouping);
        set<CardSet> boards = createCardSet(boardCount, grouping);

        for (auto pit = pockets.begin(); pit != pockets.end(); pit++)
            for (auto bit = boards.begin(); bit != boards.end(); bit++)
            {
                if (ranks)
                {
                    PokerEvaluation eval = evaluator->evaluateRanks(*pit, *bit);
                    cout << pit->str() << ", " << bit->str() << ": ["
                         << setw(4) << pit->rankColex() << "," << setw(4) << bit->rankColex()
                         << "] -> " << eval.str() << " [" << setw(9) << eval.code() << "]\n";
                }
                else
                {
                    bit->canonize(*pit);
                    if (pit->intersects(*bit))
                        continue;
                    PokerHandEvaluation eval = evaluator->evaluate(*pit, *bit);
                    cout << pit->str() << ", " << bit->str() << ": ["
                         << setw(4) << pit->colex() << "," << setw(4) << bit->colex()
                         << "] -> " << eval.str() << " [" << setw(9) << eval.high().code() << "]\n";
                }
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
