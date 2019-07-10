cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(couchbase-lite-cpp
        LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FeatureSummary)

option(BUILD_EXAMPLES "Build project examples" OFF)
add_feature_info(BUILD_EXAMPLES BUILD_EXAMPLES "Build project examples")

add_subdirectory(vendor)

set(CB_DIR "vendor/couchbase-lite-core")

add_library(${PROJECT_NAME}
    src/SGDatabase.cpp
    src/SGDocument.cpp
    src/SGMutableDocument.cpp
    src/SGReplicator.cpp
    src/SGReplicatorConfiguration.cpp
    src/SGURLEndpoint.cpp
    src/SGBasicAuthenticator.cpp
    src/SGUtility.cpp
    #HACK: This is not included in couchbase-lite-core. But, we need it for the replicator
    ${CB_DIR}/Replicator/CivetWebSocket.cc
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "include"

    # FIXME: update couchbase-lite-cpp include paths with respect to CMake imported target!!
    PUBLIC "${CB_DIR}/C/include"
    PUBLIC "${CB_DIR}/vendor/fleece/API"
    PUBLIC "${CB_DIR}/vendor/fleece/Fleece/Core"
    PUBLIC "${CB_DIR}/vendor/fleece/Fleece/Mutable"
    PUBLIC "${CB_DIR}/vendor/fleece/Fleece/Support"
    PRIVATE "${CB_DIR}/Replicator"
    PRIVATE "${CB_DIR}/LiteCore/Support"
    PRIVATE "${CB_DIR}/vendor/BLIP-Cpp/include/blip_cpp"
    PRIVATE "${CB_DIR}/vendor/BLIP-Cpp/src/util"
    PRIVATE "${CB_DIR}/vendor/civetweb/include"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC cblitecore::LiteCore
    PUBLIC cblitecore::Fleece
    PUBLIC cblitecore::CivetWeb
    PUBLIC cblitecore::BLIP
    PUBLIC cblitecore::support
    PUBLIC cblitecore::SQLite3_unicode
)
if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC cblitecore::zlib
        PUBLIC cblitecore::mbedtls
    )
else()
    target_link_libraries(${PROJECT_NAME}
        PRIVATE z
    )
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_compile_options(${PROJECT_NAME} PUBLIC -stdlib=libc++)
    target_link_libraries(${PROJECT_NAME} PRIVATE c++abi -stdlib=libc++)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
