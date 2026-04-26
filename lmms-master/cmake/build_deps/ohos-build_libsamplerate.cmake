cmake_minimum_required(VERSION 3.13)
project(ohos-build-libsamplerate)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libsamplerate")
set(BUILD_DIR "${CMAKE_BINARY_DIR}/libsamplerate-ohos")
set(INSTALL_DIR "${CMAKE_BINARY_DIR}/ohos-deps/libsamplerate")

include(ExternalProject)
ExternalProject_Add(libsamplerate_ohos
    SOURCE_DIR ${SRC_DIR}
    BINARY_DIR ${BUILD_DIR}
    INSTALL_DIR ${INSTALL_DIR}
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/ohos-toolchain.cmake
        -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
        -DCMAKE_BUILD_TYPE=Release
        -DLIBSAMPLERATE_TESTS=OFF
)
