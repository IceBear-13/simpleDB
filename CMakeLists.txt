cmake_minimum_required(VERSION 3.10)
project(SimpleDBMS)

set(CMAKE_CXX_STANDARD 17)

set(SRC_FILES
    src/main.cpp
)

add_executable(simpledbms ${SRC_FILES})

# Headers are in includes/
target_include_directories(simpledbms PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)