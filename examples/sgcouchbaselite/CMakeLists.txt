cmake_minimum_required (VERSION 3.5)

project(sgcouchbaselite-playground)

set(CMAKE_CXX_STANDARD 11)

add_executable(${PROJECT_NAME} sgcouchbaselite.cpp)

target_link_libraries(${PROJECT_NAME}
    couchbase-lite-cpp
)
