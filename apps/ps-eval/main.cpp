#include "../cli_options.h"
#include <iostream>
#include <penum/ShowdownEnumerator.h>
#include <vector>

using namespace pokerstove;
using namespace std;

int main(int argc, char** argv)
{
    try
    {
        cxxopts::Options options("ps-eval", "A poker hand evaluator");
        options.add_options()
            ("help", "produce help message")
            ("g,game", "game to use for evaluation", cxxopts::value<string>()->default_value("h"))
            ("b,board", "community cards for he/o/o8", cxxopts::value<string>())
            ("h,hand", "a hand for evaluation", cxxopts::value<vector<string>>())
            ("q,quiet", "produces no output");
        options.parse_positional({"hand"});
        options.positional_help("HAND [HAND ...]");
        const auto vm = cli::parse(options, argc, argv, {"game", "board"});
        if (vm.count("help") || argc == 1)
        {
            cout << options.help() << endl;
            return 1;
        }

        string game = vm["game"].as<string>();
        string board = vm.count("board") ? vm["board"].as<string>() : "";
        vector<string> hands;
        // Preserve each original token: commas belong to CardDistribution syntax,
        // not to cxxopts' vector delimiter. Also preserve mixed named/positional order.
        for (const auto& argument : vm.arguments())
            if (argument.key() == "hand")
                hands.push_back(argument.value());
        if (hands.empty())
            throw std::invalid_argument("At least one hand is required");
        bool quiet = vm["quiet"].as<bool>();

        // allocate evaluator and create card distributions
        std::shared_ptr<PokerHandEvaluator> evaluator = PokerHandEvaluator::alloc(game);
        if (!evaluator)
            throw std::invalid_argument("Unsupported game: " + game);
        vector<CardDistribution> handDists;
        for (const string& hand : hands)
        {
            handDists.emplace_back();
            if (!handDists.back().parse(hand))
                throw std::invalid_argument("Invalid hand distribution: " + hand);
        }

        // fill with random if necessary
        if (handDists.size() == 1)
        {
            handDists.emplace_back();
            handDists.back().fill(evaluator->handSize());
        }

        // calcuate the results and print them
        ShowdownEnumerator showdown;
        vector<EquityResult> results =
            showdown.calculateEquity(handDists, CardSet(board), evaluator);

        double total = 0.0;
        for (const EquityResult& result : results)
        {
            total += result.winShares + result.tieShares;
        }

        if (!quiet)
        {
            for (size_t i = 0; i < results.size(); ++i)
            {
                double equity =
                    (results[i].winShares + results[i].tieShares) / total;
                string handDesc =
                    (i < hands.size()) ? "The hand " + hands[i] : "A random hand";
                cout << handDesc << " has " << equity * 100. << " % equity ("
                     << results[i].str() << ")" << endl;
            }
        }
    }
    catch (const std::exception& error)
    {
        cerr << "-- caught exception--\n" << error.what() << "\n";
        return 1;
    }
    return 0;
}
