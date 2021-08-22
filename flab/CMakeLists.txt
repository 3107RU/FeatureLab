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

project(flab)

set(SOURCES main.cpp dataset.cpp)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

#pugixml
target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{UVODRIVER_SDK}/pugixml/include")
#sciter
target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{UVODRIVER_SDK}/sciter/include")
add_custom_target(${PROJECT_NAME}Resources
        COMMAND $ENV{UVODRIVER_SDK}/sciter/bin.win/packfolder.exe
        ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_SOURCE_DIR}/resources.cpp
)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}Resources)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different
$ENV{UVODRIVER_SDK}/sciter/bin.win/x64/sciter.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>)
