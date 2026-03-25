cmake_minimum_required(VERSION 3.10)
project(SimpleDBMS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Static linking
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(BUILD_SHARED_LIBS OFF)

# Source files
set(SRC_FILES src/main.cpp)

add_executable(simpledbms ${SRC_FILES})
target_include_directories(simpledbms PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)

# Install targets
install(TARGETS simpledbms 
        RUNTIME DESTINATION bin)

install(FILES README.md LICENSE CHANGELOG.md
        DESTINATION share/doc/simpledbms)

install(DIRECTORY includes/
        DESTINATION include/simpledbms)