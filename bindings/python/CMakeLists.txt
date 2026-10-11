find_package(SWIG 4.0 REQUIRED)
include(UseSWIG)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)

set(python_root ${PROJECT_BINARY_DIR}/python)
set(python_package ${python_root}/pokerstove)
set_source_files_properties(pokerstove.i PROPERTIES
    CPLUSPLUS ON
    SWIG_MODULE_NAME pokerstove
    USE_TARGET_INCLUDE_DIRECTORIES TRUE)
# SWIG's word-size definition describes C long, which is 64-bit on LP64 Unix.
if(UNIX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set_property(SOURCE pokerstove.i APPEND PROPERTY COMPILE_DEFINITIONS SWIGWORDSIZE64)
endif()
swig_add_library(pokerstove TYPE MODULE LANGUAGE python
    OUTPUT_DIR ${python_package}
    OUTFILE_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated
    SOURCES pokerstove.i)
target_compile_features(pokerstove PRIVATE cxx_std_20)
set_target_properties(pokerstove PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
    SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE
    LIBRARY_OUTPUT_DIRECTORY ${python_package})
# SWIG needs the interface headers as well as the compiler's transitive includes.
target_include_directories(pokerstove PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(pokerstove PRIVATE peval penum Python3::Module)
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(pokerstove PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY_${config} ${python_package})
endforeach()
file(GENERATE OUTPUT ${python_package}/__init__.py
    CONTENT "__version__ = \"${PROJECT_VERSION}\"\nfrom .pokerstove import *\n")
install(TARGETS pokerstove LIBRARY DESTINATION pokerstove COMPONENT python)
install(FILES ${python_package}/__init__.py ${python_package}/pokerstove.py
    DESTINATION pokerstove COMPONENT python)
if(BUILD_TESTING)
    add_test(NAME TestPython COMMAND ${CMAKE_COMMAND} -E env
        "PYTHONPATH=${python_root}" ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/test-python)
endif()
