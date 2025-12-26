cmake_minimum_required(VERSION 3.10)
project(collatz)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(collatz collatz.cpp)

# Copy gnuplot will be generated at runtime, so no install needed
