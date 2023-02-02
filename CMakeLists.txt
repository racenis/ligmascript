cmake_minimum_required(VERSION 3.20)
project(ligmascript)

set(CMAKE_CXX_STANDARD 20)

add_executable(ligmascript main.cpp ligma.h instruction_literals.cpp decompile.cpp compile.cpp interpret.cpp word.cpp ligma_wrapper.h ligma_wrapper.cpp)
