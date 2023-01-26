cmake_minimum_required(VERSION 3.18)
project(cppBlink)

set(CMAKE_CXX_STANDARD 17)

add_executable(cppBlink main.cpp)
target_link_libraries(cppBlink gpiodcxx)
