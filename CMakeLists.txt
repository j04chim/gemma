cmake_minimum_required(VERSION 3.23)

project(Gemma CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DLOG_LEVEL=0)


add_executable( Gemma )

target_sources( Gemma
    PRIVATE
        src/main.cpp
        src/gemma.cpp
        include/logger/logger.cpp
)

target_include_directories( Gemma
    PRIVATE
        include/logger
)
