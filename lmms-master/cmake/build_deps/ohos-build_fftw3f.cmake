cmake_minimum_required(VERSION 3.13)
project(ohos-build-fftw3f)

set(FFTW_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fftw3")
set(FFTW_BUILD_DIR "${CMAKE_BINARY_DIR}/fftw3f-ohos")
set(FFTW_INSTALL_DIR "${CMAKE_BINARY_DIR}/ohos-deps/fftw3f")

include(ExternalProject)
ExternalProject_Add(fftw3f_ohos
    SOURCE_DIR ${FFTW_SOURCE_DIR}
    BINARY_DIR ${FFTW_BUILD_DIR}
    INSTALL_DIR ${FFTW_INSTALL_DIR}
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/ohos-toolchain.cmake
        -DCMAKE_INSTALL_PREFIX=${FFTW_INSTALL_DIR}
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_SHARED_LIBS=OFF
        -DENABLE_FLOAT=ON
        -DENABLE_THREADS=ON
        -DENABLE_MPI=OFF
        -DENABLE_TESTS=OFF
)
