add_executable(util_tests combinations.test.cpp lastbit.test.cpp)
target_link_libraries(util_tests PRIVATE peval GTest::gtest_main)
set_target_properties(util_tests PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
add_test(NAME TestUtil COMMAND util_tests)
