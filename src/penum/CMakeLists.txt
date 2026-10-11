add_library(penum
    CardDistribution.cpp
    ShowdownEnumerator.cpp
)
add_library(pokerstove::penum ALIAS penum)
pokerstove_configure_library(penum)
target_link_libraries(penum PUBLIC peval)
