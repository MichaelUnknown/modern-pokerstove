add_library(peval
    Card.cpp
    CardSet.cpp
    CardSetGenerators.cpp
    PokerEvaluation.cpp
    PokerHand.cpp
    PokerHandEvaluator.cpp
    PokerHandEvaluator_Alloc.cpp
    Rank.cpp
    Suit.cpp
)
add_library(pokerstove::peval ALIAS peval)
pokerstove_configure_library(peval)
