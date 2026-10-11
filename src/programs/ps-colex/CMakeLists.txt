add_executable(ps-colex main.cpp)
target_link_libraries(ps-colex PRIVATE peval Boost::program_options)
set_target_properties(ps-colex PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
install(TARGETS ps-colex RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT applications)
