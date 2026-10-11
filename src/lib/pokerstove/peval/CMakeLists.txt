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

if(BUILD_TESTING)
    add_executable(peval_tests
        Card.test.cpp
        CardSet.test.cpp
        CardSetGenerators.test.cpp
        DeuceToSevenHandEvaluator.test.cpp
        HoldemHandEvaluator.test.cpp
        OmahaEightHandEvaluator.test.cpp
        OmahaHighHandEvaluator.test.cpp
        PokerEvaluation.test.cpp
        PokerHandEvaluator.test.cpp
        Rank.test.cpp
        Suit.test.cpp
        UniversalHandEvaluator.test.cpp
    )
    target_link_libraries(peval_tests PRIVATE peval GTest::gtest_main)
    set_target_properties(peval_tests PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME TestPeval COMMAND peval_tests)
endif()
