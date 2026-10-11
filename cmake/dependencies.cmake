include(${CMAKE_CURRENT_LIST_DIR}/cpm.cmake)

# Immutable upstream revisions, with versions retained for CPM/package metadata.
CPMAddPackage(NAME PackageProject.cmake VERSION 1.13.0
    GITHUB_REPOSITORY TheLartians/PackageProject.cmake
    GIT_TAG 738d2ed9dba67e01ac2ea2a6d0f8d35c198fed32)

if(MODERN_POKERSTOVE_BUILD_APPS)
    CPMAddPackage(NAME cxxopts VERSION 3.3.1
        GITHUB_REPOSITORY jarro2783/cxxopts
        GIT_TAG 44380e5a44706ab7347f400698c703eb2a196202
        OPTIONS "CXXOPTS_BUILD_EXAMPLES OFF" "CXXOPTS_BUILD_TESTS OFF"
                "CXXOPTS_ENABLE_INSTALL OFF" "CXXOPTS_ENABLE_WARNINGS OFF")
endif()

if(BUILD_TESTING)
    CPMAddPackage(NAME googletest VERSION 1.14.0
        GITHUB_REPOSITORY google/googletest
        GIT_TAG f8d7d77c06936315286eb55f8de22cd23c188571
        OPTIONS "INSTALL_GTEST OFF" "BUILD_GMOCK OFF" "gtest_force_shared_crt ON")
endif()
