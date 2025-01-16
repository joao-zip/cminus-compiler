cmake_minimum_required(VERSION 3.0)

# Project definitions

project(
  CMinusCompiler
  VERSION 1.0
  DESCRIPTION "My implementation of a C- compiler for the compiler class"
  LANGUAGES C
)

# Useful warnings

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
add_compile_options(
  -Wall
  -Wextra
)

# The project, finally

include_directories(. "src/")

file(GLOB_RECURSE SRCFILES "src/*.c")

add_executable(cmc ${SRCFILES})
