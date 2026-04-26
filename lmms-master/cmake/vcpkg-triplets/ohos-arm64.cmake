set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CMAKE_SYSTEM_NAME OHOS)
set(VCPKG_CMAKE_SYSTEM_VERSION 1)
set(VCPKG_CROSSCOMPILING ON)

set(VCPKG_CMAKE_CONFIGURE_OPTIONS
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_LIST_DIR}/../ohos-toolchain.cmake
    -DCMAKE_C_FLAGS="--target=aarch64-linux-ohos -march=armv8-a"
    -DCMAKE_CXX_FLAGS="--target=aarch64-linux-ohos -march=armv8-a"
)

set(VCPKG_LINKER_FLAGS "--target=aarch64-linux-ohos")
