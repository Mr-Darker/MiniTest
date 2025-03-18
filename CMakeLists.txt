cmake_minimum_required(VERSION 3.30)
project(MiniTest)

set(CMAKE_CXX_STANDARD 20)

# 确保 MSVC 使用 UTF-8
add_compile_options(/utf-8)

include_directories(include)
add_executable(MiniTest main.cpp)
