cmake_minimum_required(VERSION 3.30)
project(MiniTest)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
add_executable(MiniTest main.cpp)

# 如果需要设置文件编码为 UTF-8，可以使用以下选项
target_compile_options(MiniTest PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)