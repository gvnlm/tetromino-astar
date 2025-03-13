cmake_minimum_required(VERSION 3.20)
project(tetromino_astar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.cpp")

set(EXECUTABLE "tetromino_astar")

add_executable(${EXECUTABLE} ${SOURCES})

target_include_directories(${EXECUTABLE} PRIVATE include)

if (CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU|Intel|MinGW)$")
  target_compile_options(${EXECUTABLE} PRIVATE -O2 -DNDEBUG)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(${EXECUTABLE} PRIVATE /O2 /DNDEBUG)
else()
  message(WARNING "Unable to apply optimisation flags since compiler not recognised")
endif()
