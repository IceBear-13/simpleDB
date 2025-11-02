cmake_minimum_required(VERSION 3.10)
project(SimpleDBMS)

set(CMAKE_CXX_STANDARD 17)

add_executable(simpledbms
    main.cpp
    storage_engine.h
    query_processor.h
    dbms.h
)

target_include_directories(simpledbms PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})