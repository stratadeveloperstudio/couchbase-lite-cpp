include(ExternalProject)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/include)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/include/litecore)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/include/fleece)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/lib)

set(SOURCE_DIR_EXTERN ${CMAKE_SOURCE_DIR}/vendor)
#
# couchbase-lite-core
#
if(NOT WIN32)
    ExternalProject_Add(${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore
        INSTALL_DIR ${CMAKE_BINARY_DIR}/3rd_party
        SOURCE_DIR ${SOURCE_DIR_EXTERN}/couchbase-lite-core

        PATCH_COMMAND cd vendor/mbedtls && git reset --hard
        COMMAND git apply -v ${CMAKE_SOURCE_DIR}/patches/couchbase-lite-core_vendor_mbedtls_fix.patch

        CMAKE_ARGS ${CMAKE_ARGS}
        -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DENABLE_TESTING=off
        -DENABLE_PROGRAMS=off
        -DSSL_LIB=${SSL_LIB}
        -DCRYPTO_LIB=${CRYPTO_LIB}

        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_STATIC_LIBRARY_PREFIX}LiteCoreStatic${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_STATIC_LIBRARY_PREFIX}Support${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different vendor/fleece/${CMAKE_STATIC_LIBRARY_PREFIX}FleeceStatic${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different vendor/BLIP-Cpp/${CMAKE_STATIC_LIBRARY_PREFIX}BLIPStatic${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different vendor/sqlite3-unicodesn/${CMAKE_STATIC_LIBRARY_PREFIX}SQLite3_UnicodeSN${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different REST/${CMAKE_STATIC_LIBRARY_PREFIX}CivetWeb${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different REST/${CMAKE_STATIC_LIBRARY_PREFIX}LiteCoreREST_Static${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib

        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/C/include <INSTALL_DIR>/include/litecore
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/Replicator <INSTALL_DIR>/include/litecore
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/LiteCore/Support <INSTALL_DIR>/include/litecore
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/vendor/fleece/API/fleece <INSTALL_DIR>/include/fleece
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/vendor/fleece/Fleece/Core <INSTALL_DIR>/include/fleece
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/vendor/fleece/Fleece/Mutable <INSTALL_DIR>/include/fleece
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/vendor/fleece/Fleece/Support <INSTALL_DIR>/include/fleece
    )
else()
    ExternalProject_Add(${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore
        INSTALL_DIR ${CMAKE_BINARY_DIR}/3rd_party
        SOURCE_DIR ${SOURCE_DIR_EXTERN}/couchbase-lite-core

        PATCH_COMMAND cd vendor/mbedtls && git reset --hard
        COMMAND git apply -v ${CMAKE_SOURCE_DIR}/patches/couchbase-lite-core_vendor_mbedtls_fix.patch

        CMAKE_ARGS ${CMAKE_ARGS}
        -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DENABLE_TESTING=off
        -DENABLE_PROGRAMS=off
        -DSSL_LIB=${SSL_LIB}
        -DCRYPTO_LIB=${CRYPTO_LIB}

        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}LiteCoreStatic${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Support${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different vendor/fleece/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}FleeceStatic${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different vendor/BLIP-Cpp/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}BLIPStatic${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different vendor/BLIP-Cpp/vendor/zlib/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic$<$<CONFIG:Debug>:d>${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib/
        COMMAND ${CMAKE_COMMAND} -E copy_if_different vendor/sqlite3-unicodesn/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}SQLite3_UnicodeSN${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different vendor/mbedtls/library/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different REST/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}CivetWeb${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different REST/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}LiteCoreREST_Static${CMAKE_STATIC_LIBRARY_SUFFIX} <INSTALL_DIR>/lib

        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/C/include <INSTALL_DIR>/include/litecore
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/Replicator <INSTALL_DIR>/include/litecore
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/LiteCore/Support <INSTALL_DIR>/include/litecore
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/vendor/fleece/API/fleece <INSTALL_DIR>/include/fleece
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/vendor/fleece/Fleece/Core <INSTALL_DIR>/include/fleece
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/vendor/fleece/Fleece/Mutable <INSTALL_DIR>/include/fleece
        COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/vendor/fleece/Fleece/Support <INSTALL_DIR>/include/fleece
    )
endif()

add_library(cblitecore::LiteCore STATIC IMPORTED GLOBAL)
set_target_properties(cblitecore::LiteCore PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/3rd_party/include"
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/3rd_party/lib/${CMAKE_STATIC_LIBRARY_PREFIX}LiteCoreStatic${CMAKE_STATIC_LIBRARY_SUFFIX}"
)
add_dependencies(cblitecore::LiteCore DEPENDS ${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore)

add_library(cblitecore::Fleece STATIC IMPORTED GLOBAL)
set_target_properties(cblitecore::Fleece PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/3rd_party/include"
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/3rd_party/lib/${CMAKE_STATIC_LIBRARY_PREFIX}FleeceStatic${CMAKE_STATIC_LIBRARY_SUFFIX}"
)
add_dependencies(cblitecore::Fleece DEPENDS ${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore)

add_library(cblitecore::CivetWeb STATIC IMPORTED GLOBAL)
set_target_properties(cblitecore::CivetWeb PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/3rd_party/include"
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/3rd_party/lib/${CMAKE_STATIC_LIBRARY_PREFIX}CivetWeb${CMAKE_STATIC_LIBRARY_SUFFIX}"
)
add_dependencies(cblitecore::CivetWeb DEPENDS ${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore)

add_library(cblitecore::LiteCoreRest STATIC IMPORTED GLOBAL)
set_target_properties(cblitecore::LiteCoreRest PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/3rd_party/include"
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/3rd_party/lib/${CMAKE_STATIC_LIBRARY_PREFIX}LiteCoreREST_Static${CMAKE_STATIC_LIBRARY_SUFFIX}"
)
add_dependencies(cblitecore::LiteCoreRest DEPENDS ${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore)

add_library(cblitecore::BLIP STATIC IMPORTED GLOBAL)
set_target_properties(cblitecore::BLIP PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/3rd_party/include"
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/3rd_party/lib/${CMAKE_STATIC_LIBRARY_PREFIX}BLIPStatic${CMAKE_STATIC_LIBRARY_SUFFIX}"
)
add_dependencies(cblitecore::BLIP DEPENDS ${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore)

add_library(cblitecore::support STATIC IMPORTED GLOBAL)
set_target_properties(cblitecore::support PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/3rd_party/include"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/3rd_party/include/fleece"
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/3rd_party/lib/${CMAKE_STATIC_LIBRARY_PREFIX}Support${CMAKE_STATIC_LIBRARY_SUFFIX}"
)
add_dependencies(cblitecore::support DEPENDS ${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore)

add_library(cblitecore::SQLite3_unicode STATIC IMPORTED GLOBAL)
set_target_properties(cblitecore::SQLite3_unicode PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/3rd_party/include"
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/3rd_party/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SQLite3_UnicodeSN${CMAKE_STATIC_LIBRARY_SUFFIX}"
)
add_dependencies(cblitecore::SQLite3_unicode DEPENDS ${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore)

if(WIN32)
    add_library(cblitecore::zlib STATIC IMPORTED GLOBAL)
    set_target_properties(cblitecore::zlib PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/3rd_party/include"
        IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/3rd_party/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}"
        IMPORTED_LOCATION_DEBUG "${CMAKE_BINARY_DIR}/3rd_party/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstaticd${CMAKE_STATIC_LIBRARY_SUFFIX}"
    )
    add_dependencies(cblitecore::zlib DEPENDS ${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore)

    add_library(cblitecore::mbedtls STATIC IMPORTED GLOBAL)
    set_target_properties(cblitecore::mbedtls PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/3rd_party/include"
        IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/3rd_party/lib/${CMAKE_STATIC_LIBRARY_PREFIX}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX}"
    )
    add_dependencies(cblitecore::mbedtls DEPENDS ${CMAKE_STATIC_LIBRARY_PREFIX}cblitecore)
endif()