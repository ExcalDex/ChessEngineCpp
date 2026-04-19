cmake_minimum_required(VERSION 3.10)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chess
    src/main.cpp
    src/core/Board.cpp
    src/core/Game.cpp
    src/ui/ChessUI.cpp
        src/core/Engine.hpp
)

target_include_directories(chess PRIVATE src)