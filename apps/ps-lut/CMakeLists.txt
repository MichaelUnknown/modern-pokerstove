add_executable(ps-lut main.cpp)
target_link_libraries(ps-lut PRIVATE peval Boost::program_options)
set_target_properties(ps-lut PROPERTIES
    CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
install(TARGETS ps-lut RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT applications)
