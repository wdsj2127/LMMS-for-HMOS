cmake_minimum_required(VERSION 3.13)
project(ohos-build-fluidsynth)

set(FLUID_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fluidsynth")
set(BUILD_DIR "${CMAKE_BINARY_DIR}/fluidsynth-ohos")
set(INSTALL_DIR "${CMAKE_BINARY_DIR}/ohos-deps/fluidsynth")

include(ExternalProject)
ExternalProject_Add(fluidsynth_ohos
    SOURCE_DIR ${FLUID_DIR}
    BINARY_DIR ${BUILD_DIR}
    INSTALL_DIR ${INSTALL_DIR}
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/ohos-toolchain.cmake
        -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_SHARED_LIBS=OFF
        -Denable-tests=OFF
        -Denable-examples=OFF
        -Denable-docs=OFF
        -Denable-network=OFF
        -Denable-ipv6=OFF
        -Denable-jack=OFF
        -Denable-alsa=OFF
        -Denable-pulseaudio=OFF
        -Denable-sdl2=OFF
        -Denable-portaudio=OFF
        -Denable-dbus=OFF
        -Denable-ladspa=OFF
        -Denable-aufile=OFF
        -Denable-midishare=OFF
        -Denable-oss=OFF
        -Denable-pkgconfig=OFF
        -Denable-openmp=OFF
        -Denable-threads=ON
)
