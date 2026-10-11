# Header-only cxxopts 3.3.1, pinned and licensed in this directory.
add_library(cxxopts INTERFACE)
add_library(cxxopts::cxxopts ALIAS cxxopts)
target_include_directories(cxxopts SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
install(FILES LICENSE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/pokerstove/cxxopts
    COMPONENT applications)
