cmake_minimum_required(VERSION 3.16)
project(CVM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(cvm ${SOURCES})
target_include_directories(cvm PRIVATE include)

if (MSVC)
    target_compile_options(cvm PRIVATE /W4)
else()
    target_compile_options(cvm PRIVATE -Wall -Wextra)
endif()

message(STATUS "CVM++ configured.")
