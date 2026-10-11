add_executable(ps-eval main.cpp)
target_link_libraries(ps-eval PRIVATE penum Boost::program_options)
set_target_properties(ps-eval PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
install(TARGETS ps-eval RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT applications)
