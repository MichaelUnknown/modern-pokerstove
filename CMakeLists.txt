cmake_minimum_required(VERSION 3.24)

project(pokerstove VERSION 1.2 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Preserve the packaging switches used by older callers.
option(BUILD_WHEEL "Build a Python wheel (bindings only by default)" OFF)
option(BUILD_PYTHON "Enable Python bindings (legacy alias)" OFF)
if(BUILD_WHEEL)
    set(component_default OFF)
else()
    set(component_default ON)
endif()
option(MODERN_POKERSTOVE_BUILD_APPS "Build command-line programs" ${component_default})
option(BUILD_TESTING "Build tests" ${component_default})
option(MODERN_POKERSTOVE_BUILD_PYTHON "Build Python bindings" ${BUILD_PYTHON})
if(BUILD_WHEEL OR BUILD_PYTHON)
    set(MODERN_POKERSTOVE_BUILD_PYTHON ON)
endif()
include(CTest)

# Only command-line parsing needs Boost; core targets have no Boost dependency.
if(MODERN_POKERSTOVE_BUILD_APPS)
    find_package(Boost 1.71 CONFIG REQUIRED COMPONENTS program_options)
endif()

function(pokerstove_configure_library target)
    target_compile_features(${target} PUBLIC cxx_std_20)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
        POSITION_INDEPENDENT_CODE ON)
    target_include_directories(${target} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
endfunction()

if(BUILD_TESTING)
    # Use the checked-in dependency; do not install it with the core package.
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(third_party/googletest EXCLUDE_FROM_ALL)
endif()

add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
if(MODERN_POKERSTOVE_BUILD_APPS)
    add_subdirectory(apps)
endif()
if(MODERN_POKERSTOVE_BUILD_PYTHON)
    add_subdirectory(bindings/python)
endif()

install(TARGETS peval penum EXPORT pokerstoveTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT development)
# Only the public header tree is installed.
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT development FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
set(package_dir ${CMAKE_INSTALL_LIBDIR}/cmake/pokerstove)
configure_package_config_file(cmake/pokerstoveConfig.cmake.in
    ${PROJECT_BINARY_DIR}/pokerstoveConfig.cmake
    INSTALL_DESTINATION ${package_dir})
write_basic_package_version_file(${PROJECT_BINARY_DIR}/pokerstoveConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(EXPORT pokerstoveTargets NAMESPACE pokerstove::
    DESTINATION ${package_dir} COMPONENT development)
install(FILES ${PROJECT_BINARY_DIR}/pokerstoveConfig.cmake
    ${PROJECT_BINARY_DIR}/pokerstoveConfigVersion.cmake
    DESTINATION ${package_dir} COMPONENT development)
