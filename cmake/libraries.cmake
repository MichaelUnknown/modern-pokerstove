add_library(peval
    src/peval/Card.cpp
    src/peval/CardSet.cpp
    src/peval/CardSetGenerators.cpp
    src/peval/PokerEvaluation.cpp
    src/peval/PokerHand.cpp
    src/peval/PokerHandEvaluator.cpp
    src/peval/PokerHandEvaluator_Alloc.cpp
    src/peval/Rank.cpp
    src/peval/Suit.cpp)
add_library(penum src/penum/CardDistribution.cpp src/penum/ShowdownEnumerator.cpp)
target_link_libraries(penum PUBLIC peval)

foreach(target IN ITEMS peval penum)
    target_compile_features(${target} PUBLIC cxx_std_20)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
        POSITION_INDEPENDENT_CODE ON
        VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR}
        WINDOWS_EXPORT_ALL_SYMBOLS ON)
    target_include_directories(${target} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
endforeach()

# Aggregate package preserves find_package(pokerstove) and both component targets.
add_library(pokerstove INTERFACE)
target_link_libraries(pokerstove INTERFACE peval penum)

# Installed shared libraries locate their siblings without environment overrides.
if(APPLE)
    set(pokerstove_loader_origin "@loader_path")
elseif(UNIX)
    set(pokerstove_loader_origin "$ORIGIN")
endif()
if(BUILD_SHARED_LIBS AND DEFINED pokerstove_loader_origin)
    set_target_properties(peval penum PROPERTIES
        INSTALL_RPATH "${pokerstove_loader_origin}")
endif()
