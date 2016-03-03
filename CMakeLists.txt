cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Chess)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -DNDEBUG -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -DNDEBUG -Wall")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -ggdb -Wall")

add_executable(chess bitfunc.h Board.cpp Board.h directions.h Evaluate.cpp Evaluate.h game.cpp magicmoves.c magicmoves.h Movegen.cpp Movegen.h Search.cpp Search.h Test.cpp Test.h ToString.cpp ToString.h types.h Uci.cpp Uci.h)
