function(pokerstove_add_component_tests component library test_name)
    add_executable(${component}_tests ${ARGN})
    target_link_libraries(${component}_tests PRIVATE ${library} GTest::gtest_main)
    set_target_properties(${component}_tests PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME ${test_name} COMMAND ${component}_tests)
endfunction()

pokerstove_add_component_tests(peval peval TestPeval
    tests/peval/CardSetGenerators_test.cpp
    tests/peval/CardSet_test.cpp
    tests/peval/Card_test.cpp
    tests/peval/DeuceToSevenHandEvaluator_test.cpp
    tests/peval/HoldemHandEvaluator_test.cpp
    tests/peval/OmahaEightHandEvaluator_test.cpp
    tests/peval/OmahaHighHandEvaluator_test.cpp
    tests/peval/PokerEvaluation_test.cpp
    tests/peval/PokerHandEvaluator_test.cpp
    tests/peval/Rank_test.cpp
    tests/peval/Suit_test.cpp
    tests/peval/UniversalHandEvaluator_test.cpp
)

pokerstove_add_component_tests(penum penum TestPenum
    tests/penum/card_distribution_test.cpp
    tests/penum/PartitionEnumerator_test.cpp
    tests/penum/SimpleDeck_test.cpp
)

pokerstove_add_component_tests(util peval TestUtil
    tests/util/combinations_test.cpp
    tests/util/lastbit_test.cpp
)

if(MODERN_POKERSTOVE_BUILD_APPS)
    add_test(NAME TestCLI COMMAND ${CMAKE_COMMAND}
        -DPS_EVAL=$<TARGET_FILE:ps-eval>
        -DPS_COLEX=$<TARGET_FILE:ps-colex>
        -DPS_LUT=$<TARGET_FILE:ps-lut>
        -P ${PROJECT_SOURCE_DIR}/tests/integration/check_cli.cmake)
endif()
