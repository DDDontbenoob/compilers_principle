cmake_minimum_required(VERSION 3.21)
project(compilers_principle_master)

set(CMAKE_CXX_STANDARD 14)

add_executable(compilers_principle_master
        SLR/dictree.cpp
        SLR/Grammer.cpp
        main.cpp
        SLR/SLR.cpp LEX/LEX.cpp LEX/LEX.h SDT/SDT.cpp SDT/SDT.h)
