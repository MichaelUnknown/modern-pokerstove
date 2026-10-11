function(pokerstove_add_tests component library test_name)
    add_executable(${component}_tests ${ARGN})
    target_link_libraries(${component}_tests PRIVATE ${library} GTest::gtest_main)
    set_target_properties(${component}_tests PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME ${test_name} COMMAND ${component}_tests)
endfunction()

pokerstove_add_tests(peval peval TestPeval
    peval/CardSetGenerators_test.cpp
    peval/CardSet_test.cpp
    peval/Card_test.cpp
    peval/DeuceToSevenHandEvaluator_test.cpp
    peval/HoldemHandEvaluator_test.cpp
    peval/OmahaEightHandEvaluator_test.cpp
    peval/OmahaHighHandEvaluator_test.cpp
    peval/PokerEvaluation_test.cpp
    peval/PokerHandEvaluator_test.cpp
    peval/Rank_test.cpp
    peval/Suit_test.cpp
    peval/UniversalHandEvaluator_test.cpp
)

pokerstove_add_tests(penum penum TestPenum
    penum/card_distribution_test.cpp
    penum/PartitionEnumerator_test.cpp
    penum/SimpleDeck_test.cpp
)

pokerstove_add_tests(util peval TestUtil
    util/combinations_test.cpp
    util/lastbit_test.cpp
)
