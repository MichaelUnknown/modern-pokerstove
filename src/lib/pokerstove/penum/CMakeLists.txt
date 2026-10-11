add_library(penum
    CardDistribution.cpp
    ShowdownEnumerator.cpp
)
add_library(pokerstove::penum ALIAS penum)
pokerstove_configure_library(penum)
target_link_libraries(penum PUBLIC peval)

if(BUILD_TESTING)
    add_executable(penum_tests
        PartitionEnumerator.test.cpp
        SimpleDeck.test.cpp
    )
    target_link_libraries(penum_tests PRIVATE penum GTest::gtest_main)
    set_target_properties(penum_tests PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    add_test(NAME TestPenum COMMAND penum_tests)
endif()
