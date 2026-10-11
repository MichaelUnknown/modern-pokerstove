# Account for configurable GNUInstallDirs when computing a relocatable RPATH.
get_filename_component(install_libdir "${CMAKE_INSTALL_LIBDIR}" ABSOLUTE
    BASE_DIR "${CMAKE_INSTALL_PREFIX}")
get_filename_component(install_bindir "${CMAKE_INSTALL_BINDIR}" ABSOLUTE
    BASE_DIR "${CMAKE_INSTALL_PREFIX}")
file(RELATIVE_PATH bin_to_lib "${install_bindir}" "${install_libdir}")

foreach(program IN ITEMS ps-eval ps-colex ps-lut)
    add_executable(${program} apps/${program}/main.cpp)
    target_link_libraries(${program} PRIVATE penum cxxopts::cxxopts)
    set_target_properties(${program} PROPERTIES
        CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
    if(BUILD_SHARED_LIBS AND DEFINED pokerstove_loader_origin)
        set_target_properties(${program} PROPERTIES
            INSTALL_RPATH "${pokerstove_loader_origin}/${bin_to_lib}")
    endif()
    install(TARGETS ${program}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT applications)
endforeach()
# The parser is private, but distributed executables retain its MIT notice.
install(FILES "${cxxopts_SOURCE_DIR}/LICENSE"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/pokerstove/cxxopts"
    COMPONENT applications)
