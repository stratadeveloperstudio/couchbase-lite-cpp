cmake_minimum_required (VERSION 3.8)
project(sgcouchbaselite-playground
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

add_executable(${PROJECT_NAME}
    sgcouchbaselite.cpp
    sgloggingcategories.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    CouchbaseLiteCPP
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE ${FOUNDATION_LIBRARY}
        PRIVATE ${CORE_FOUNDATION}
    )
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SHOW_DATABASE_MESSAGES)
endif()
