find_package(SWIG 4.0 REQUIRED)
include(UseSWIG)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)

set(python_root ${PROJECT_BINARY_DIR}/python)
set(python_package ${python_root}/pokerstove)
set_source_files_properties(${PROJECT_SOURCE_DIR}/bindings/python/pokerstove.i PROPERTIES
    CPLUSPLUS ON
    SWIG_MODULE_NAME pokerstove
    USE_TARGET_INCLUDE_DIRECTORIES TRUE)
# SWIG's word-size definition describes C long, which is 64-bit on LP64 Unix.
if(UNIX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set_property(SOURCE ${PROJECT_SOURCE_DIR}/bindings/python/pokerstove.i APPEND PROPERTY COMPILE_DEFINITIONS SWIGWORDSIZE64)
endif()
swig_add_library(pokerstove_python TYPE MODULE LANGUAGE python
    OUTPUT_DIR ${python_package}
    OUTFILE_DIR ${PROJECT_BINARY_DIR}/python/generated
    SOURCES ${PROJECT_SOURCE_DIR}/bindings/python/pokerstove.i)
target_compile_features(pokerstove_python PRIVATE cxx_std_20)
set_target_properties(pokerstove_python PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
    OUTPUT_NAME pokerstove
    SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE
    LIBRARY_OUTPUT_DIRECTORY ${python_package})
# SWIG needs the interface headers as well as the compiler's transitive includes.
target_include_directories(pokerstove_python PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(pokerstove_python PRIVATE peval penum Python3::Module)
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(pokerstove_python PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY_${config} ${python_package})
endforeach()
file(GENERATE OUTPUT ${python_package}/__init__.py
    CONTENT "__version__ = \"${PROJECT_VERSION}\"\nfrom .pokerstove import *\n")
if(BUILD_SHARED_LIBS AND DEFINED pokerstove_loader_origin)
    get_filename_component(install_libdir "${CMAKE_INSTALL_LIBDIR}" ABSOLUTE
        BASE_DIR "${CMAKE_INSTALL_PREFIX}")
    file(RELATIVE_PATH python_to_lib "${CMAKE_INSTALL_PREFIX}/pokerstove" "${install_libdir}")
    set_target_properties(pokerstove_python PROPERTIES
        INSTALL_RPATH "${pokerstove_loader_origin}/${python_to_lib}")
endif()
install(TARGETS pokerstove_python LIBRARY DESTINATION pokerstove COMPONENT python)
install(FILES ${python_package}/__init__.py ${python_package}/pokerstove.py
    DESTINATION pokerstove COMPONENT python)
if(BUILD_TESTING)
    add_test(NAME TestPython COMMAND ${CMAKE_COMMAND} -E env
        "PYTHONPATH=${python_root}" ${Python3_EXECUTABLE}
        ${PROJECT_SOURCE_DIR}/bindings/python/test-python)
endif()
