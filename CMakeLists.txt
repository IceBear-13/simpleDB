cmake_minimum_required(VERSION 3.10)
project(SimpleDBMS)

set(CMAKE_CXX_STANDARD 17)

# Static linking options
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

set(SRC_FILES
    src/main.cpp
)

add_executable(simpledbms ${SRC_FILES})
target_include_directories(simpledbms PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)

# Installation rules
install(TARGETS simpledbms DESTINATION bin)
install(FILES README.md LICENSE DESTINATION share/doc/simpledbms)