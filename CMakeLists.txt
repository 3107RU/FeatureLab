cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (WIN32)
add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

project(fl)
add_subdirectory(flab)
