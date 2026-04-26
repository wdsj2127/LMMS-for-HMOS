set(CMAKE_SYSTEM_NAME OHOS)
set(CMAKE_SYSTEM_VERSION 1)

if(NOT DEFINED OHOS_SDK_ROOT)
    if(DEFINED ENV{NATIVE_OHOS_SDK})
        set(OHOS_SDK_ROOT "$ENV{NATIVE_OHOS_SDK}")
    else()
        message(FATAL_ERROR "NATIVE_OHOS_SDK environment variable is not set. "
            "Please set it to the HarmonyOS NDK path, e.g. "
            "/path/to/OpenHarmony/Sdk/12/native or "
            "D:\\oh\\DevEcoStudio\\sdk\\HarmonyOS-NEXT-DB6\\openharmony\\native")
    endif()
endif()

set(OHOS_LLVM_DIR "${OHOS_SDK_ROOT}/llvm")
set(OHOS_SYSROOT "${OHOS_SDK_ROOT}/sysroot")

set(CMAKE_C_COMPILER "${OHOS_LLVM_DIR}/bin/clang")
set(CMAKE_CXX_COMPILER "${OHOS_LLVM_DIR}/bin/clang++")
set(CMAKE_AR "${OHOS_LLVM_DIR}/bin/llvm-ar")
set(CMAKE_RANLIB "${OHOS_LLVM_DIR}/bin/llvm-ranlib")
set(CMAKE_LINKER "${OHOS_LLVM_DIR}/bin/ld.lld")
set(CMAKE_NM "${OHOS_LLVM_DIR}/bin/llvm-nm")
set(CMAKE_OBJDUMP "${OHOS_LLVM_DIR}/bin/llvm-objdump")
set(CMAKE_STRIP "${OHOS_LLVM_DIR}/bin/llvm-strip")

set(CMAKE_FIND_ROOT_PATH "${OHOS_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --target=aarch64-linux-ohos -march=armv8-a")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --target=aarch64-linux-ohos -march=armv8-a")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LMMS_BUILD_OHOS 1 CACHE INTERNAL "Building for HarmonyOS (OHOS) platform")

message(STATUS "===============================================")
message(STATUS "HarmonyOS (OHOS) Cross-Compilation Toolchain")
message(STATUS "  SDK Root:      ${OHOS_SDK_ROOT}")
message(STATUS "  LLVM Dir:      ${OHOS_LLVM_DIR}")
message(STATUS "  Sysroot:       ${OHOS_SYSROOT}")
message(STATUS "  C Compiler:    ${CMAKE_C_COMPILER}")
message(STATUS "  C++ Compiler:  ${CMAKE_CXX_COMPILER}")
message(STATUS "  Target:        aarch64-linux-ohos")
message(STATUS "===============================================")
