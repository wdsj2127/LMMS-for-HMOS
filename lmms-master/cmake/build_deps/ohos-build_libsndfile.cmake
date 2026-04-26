cmake_minimum_required(VERSION 3.13)
project(ohos-build-libsndfile)

set(LIBSNDFILE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libsndfile")
set(LIBSNDFILE_BUILD_DIR "${CMAKE_BINARY_DIR}/libsndfile-ohos")
set(LIBSNDFILE_INSTALL_DIR "${CMAKE_BINARY_DIR}/ohos-deps/libsndfile")

include(ExternalProject)
ExternalProject_Add(libsndfile_ohos
    SOURCE_DIR ${LIBSNDFILE_SOURCE_DIR}
    BINARY_DIR ${LIBSNDFILE_BUILD_DIR}
    INSTALL_DIR ${LIBSNDFILE_INSTALL_DIR}
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/ohos-toolchain.cmake
        -DCMAKE_INSTALL_PREFIX=${LIBSNDFILE_INSTALL_DIR}
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_PROGRAMS=OFF
        -DBUILD_EXAMPLES=OFF
        -DBUILD_TESTING=OFF
        -DENABLE_EXTERNAL_LIBS=OFF
)
