cmake_minimum_required(VERSION 3.10)
project(GraphDB_LSM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(include)

find_package(ZLIB REQUIRED)
find_package(TBB REQUIRED)

set(SOURCES
    src/main.cpp
    src/GraphDB.cpp
    src/LSMTree.cpp
    src/MemTable.cpp
    src/SSTable.cpp
    src/Cache.cpp
)

add_executable(GraphDB ${SOURCES})

target_link_libraries(GraphDB PRIVATE TBB::tbb ZLIB::ZLIB stdc++fs)
target_include_directories(GraphDB PRIVATE include)