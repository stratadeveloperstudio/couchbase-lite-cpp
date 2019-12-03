cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(CouchbaseLiteCPP
        LANGUAGES CXX
        VERSION 1.0
)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FeatureSummary)

option(BUILD_EXAMPLES "Build project examples" ON)
add_feature_info(BUILD_EXAMPLES BUILD_EXAMPLES "Build project examples")

add_subdirectory(vendor)

set(CB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/couchbase-lite-core")

add_library(${PROJECT_NAME}
    src/SGDatabase.cpp
    src/SGDocument.cpp
    src/SGMutableDocument.cpp
    src/SGReplicator.cpp
    src/SGReplicatorConfiguration.cpp
    src/SGURLEndpoint.cpp
    src/SGBasicAuthenticator.cpp
    src/SGUtility.cpp
    src/SGPath.cpp
)

target_include_directories(${PROJECT_NAME}
     PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
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

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    LIBRARY DESTINATION lib ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION lib ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION bin ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION include ${CMAKE_INSTALL_INCLUDEDIR}
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(EXPORT ${PROJECT_NAME}
    FILE "${PROJECT_NAME}Targets.cmake"
    NAMESPACE strata::
    DESTINATION lib/cmake/${PROJECT_NAME}
)
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include/couchbaselitecpp
    FILES_MATCHING PATTERN "*.h*"
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DSHOW_DETAILED_DATABASE_MESSAGES)
endif()