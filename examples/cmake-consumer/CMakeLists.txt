cmake_minimum_required(VERSION 3.24)
project(pokerstove_consumer LANGUAGES CXX)

find_package(pokerstove 1.2 CONFIG REQUIRED)
add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE pokerstove::penum)
set_target_properties(consumer PROPERTIES CXX_EXTENSIONS NO)

enable_testing()
add_test(NAME InstalledConsumer COMMAND consumer)
