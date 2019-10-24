cmake_minimum_required (VERSION 3.8)
project(sgcouchbaselite-example
        LANGUAGES CXX
)

if(APPLE)
    find_library(FOUNDATION_LIB Foundation REQUIRED)
    if (NOT FOUNDATION_LIB)
        message(FATAL_ERROR "Foundation framework not found")
    endif()

    find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
    if (NOT CORE_FOUNDATION)
        message(FATAL_ERROR "CoreFoundation framework not found")
    endif()
endif()

add_executable(sgcouchbaselite-playground sgcouchbaselite-playground.cpp)

add_executable(sgcouchbaselite-buckets sgcouchbaselite-buckets.cpp)

target_link_libraries(sgcouchbaselite-playground PRIVATE CouchbaseLiteCPP)

target_link_libraries(sgcouchbaselite-buckets PRIVATE CouchbaseLiteCPP)

if(APPLE)
    target_link_libraries(sgcouchbaselite-playground PRIVATE ${FOUNDATION_LIBRARY} PRIVATE ${CORE_FOUNDATION})
    target_link_libraries(sgcouchbaselite-buckets PRIVATE ${FOUNDATION_LIBRARY} PRIVATE ${CORE_FOUNDATION})
endif()
