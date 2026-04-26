# LMMS for HarmonyOS - 构建指南

## 前置条件

### 1. 安装 DevEco Studio
- 下载地址: https://developer.huawei.com/consumer/cn/deveco-studio/
- 安装后通过SDK Manager确保已下载 HarmonyOS SDK (API Level 15+)
- 记录 NDK 路径，例如: `D:\oh\DevEcoStudio\sdk\HarmonyOS-NEXT-DB6\openharmony\native`

### 2. 获取 Qt 5.15.16 for HarmonyOS 源码

```bash
# 从 Qt Gerrit 获取源码
git clone https://codereview.qt-project.org/qt/tqtc-qt5
cd tqtc-qt5
git checkout tqtc/harmonyos-5.15.16
git submodule update --init --recursive
```

### 3. 设置环境变量

**Windows (PowerShell):**
```powershell
$env:NATIVE_OHOS_SDK = "D:\oh\DevEcoStudio\sdk\HarmonyOS-NEXT-DB6\openharmony\native"
$env:OHOS_SDK_SYSROOT = "$env:NATIVE_OHOS_SDK\sysroot"
$env:LLVM_INSTALL_DIR = "$env:NATIVE_OHOS_SDK\llvm"
```

**macOS/Linux:**
```bash
export NATIVE_OHOS_SDK=/path/to/OpenHarmony/Sdk/12/native
export OHOS_SDK_SYSROOT=$NATIVE_OHOS_SDK/sysroot
export LLVM_INSTALL_DIR=$NATIVE_OHOS_SDK/llvm
```

## Phase 1: 构建 Qt for HarmonyOS

### 1.1 编译 Qt 框架

**Windows:**
```cmd
SET NATIVE_OHOS_SDK=D:\oh\DevEcoStudio\sdk\HarmonyOS-NEXT-DB6\openharmony\native
SET OHOS_SDK_SYSROOT=%NATIVE_OHOS_SDK%\sysroot
SET LLVM_INSTALL_DIR=%NATIVE_OHOS_SDK%\llvm
SET QT5_ROOT_DIR=D:\oh\tqtc-qt5

SET QT_SKIPS=-skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcanvas3d -skip qtconnectivity -skip qtdatavis3d -skip qtdoc -skip qtdocgallery -skip qtfeedback -skip qtgamepad -skip qtgraphicaleffects -skip qtlocation -skip qtmacextras -skip qtnetworkauth -skip qtpim -skip qtpurchasing -skip qtqa -skip qtremoteobjects -skip qtrepotools -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtspeech -skip qtsystems -skip qttools -skip qttranslations -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebglplugin -skip qtwebsockets -skip qtwebview -skip qtwinextras -skip qtx11extras -no-dbus -skip doc

mkdir build_qt_ohos
cd build_qt_ohos

call ..\tqtc-qt5\configure.bat -v -xplatform ohos-clang -device-option CROSS_COMPILE=%LLVM_INSTALL_DIR%\bin -prefix /data/storage/el1/bundle/libs/arm64 -extprefix C:\Qt\qt-5.15.16-ohos -opensource -confirm-license -release -no-use-gold-linker %QT_SKIPS% -nomake tests -nomake examples -no-gcc-sysroot -c++std c++14 -ohos-arch arm64-v8a

mingw32-make -j8
mingw32-make install
```

**macOS:**
```bash
mkdir build_qt_ohos && cd build_qt_ohos

../tqtc-qt5/configure \
    -v \
    -xplatform ohos-clang \
    -prefix /data/storage/el1/bundle/libs/arm64 \
    -extprefix $HOME/Qt/5.15.16/ohos-arm64-clang/ \
    -opensource \
    -confirm-license \
    -release \
    -no-use-gold-linker \
    -no-gcc-sysroot \
    -ohos-arch arm64-v8a \
    $QT_SKIPS \
    -nomake examples \
    -nomake tests

make -j16
make install
```

### 1.2 验证 Qt 编译产物
检查安装目录下是否存在以下库:
- `libQt5Core.so`
- `libQt5Gui.so`
- `libQt5Widgets.so`
- `libQt5Xml.so`
- `libQt5Svg.so`

## Phase 2: 交叉编译 LMMS

### 2.1 编译第三方依赖

使用 vcpkg 为 HarmonyOS 交叉编译:

```bash
# 创建 vcpkg triplet
cat > triplets/ohos-arm64.cmake << 'EOF'
set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CMAKE_SYSTEM_NAME OHOS)
set(VCPKG_CMAKE_SYSTEM_VERSION 1)
set(VCPKG_LINKER_FLAGS "--target=aarch64-linux-ohos")
EOF

# 编译依赖
vcpkg install libsndfile:ohos-arm64 fftw3:ohos-arm64 libsamplerate:ohos-arm64 fluidsynth:ohos-arm64
```

### 2.2 配置 LMMS 构建

```bash
mkdir build_lmms_ohos && cd build_lmms_ohos

cmake ../lmms-master \
    -DCMAKE_TOOLCHAIN_FILE=../lmms-master/cmake/ohos-toolchain.cmake \
    -DLMMS_BUILD_OHOS=ON \
    -DCMAKE_PREFIX_PATH="C:/Qt/qt-5.15.16-ohos" \
    -DWANT_ALSA=OFF \
    -DWANT_OSS=OFF \
    -DWANT_PULSEAUDIO=OFF \
    -DWANT_SDL=OFF \
    -DWANT_JACK=OFF \
    -DWANT_LV2=OFF \
    -DWANT_CARLA=OFF \
    -DWANT_VST=OFF \
    -DWANT_PORTAUDIO=OFF \
    -DWANT_OHAUDIO=ON \
    -DWANT_OH_MIDI=ON \
    -DCMAKE_BUILD_TYPE=Release

cmake --build . -j8
```

### 2.3 验证 LMMS 编译产物
- `liblmms.so` - 主应用库
- `plugins/` - 插件目录下的各 .so 文件

## Phase 3: 打包 HAP

### 3.1 复制库文件到 DevEco 项目

```powershell
# 创建 libs 目录
New-Item -ItemType Directory -Force -Path "LMMS_for_HMOS\entry\libs\arm64-v8a"

# 复制 LMMS 编译产物
Copy-Item "build_lmms_ohos\liblmms.so" "LMMS_for_HMOS\entry\libs\arm64-v8a\"

# 复制 Qt 运行时库
Copy-Item "C:\Qt\qt-5.15.16-ohos\lib\libQt5Core.so" "LMMS_for_HMOS\entry\libs\arm64-v8a\"
Copy-Item "C:\Qt\qt-5.15.16-ohos\lib\libQt5Gui.so" "LMMS_for_HMOS\entry\libs\arm64-v8a\"
Copy-Item "C:\Qt\qt-5.15.16-ohos\lib\libQt5Widgets.so" "LMMS_for_HMOS\entry\libs\arm64-v8a\"
Copy-Item "C:\Qt\qt-5.15.16-ohos\lib\libQt5Xml.so" "LMMS_for_HMOS\entry\libs\arm64-v8a\"
Copy-Item "C:\Qt\qt-5.15.16-ohos\lib\libQt5Svg.so" "LMMS_for_HMOS\entry\libs\arm64-v8a\"

# 复制 Qt 插件平台
Copy-Item "C:\Qt\qt-5.15.16-ohos\plugins\platforms\libqtaf_ohos.so" "LMMS_for_HMOS\entry\libs\arm64-v8a\"

# 复制第三方依赖库
Copy-Item "vcpkg_installed\ohos-arm64\lib\*.so" "LMMS_for_HMOS\entry\libs\arm64-v8a\"
```

### 3.2 构建并运行 HAP

1. 用 DevEco Studio 打开 `LMMS_for_HMOS` 项目
2. 配置签名: File → Project Structure → Signing Configs
3. 连接鸿蒙设备
4. 点击运行按钮构建并安装 HAP
5. 验证 LMMS 主窗口界面显示

## 常见问题

### Qt configure 失败
- 确保 NATIVE_OHOS_SDK 环境变量正确
- 确保使用的 HarmonyOS SDK API Level >= 15
- 检查 llvm-mingw 工具链是否正确安装 (Windows)

### LMMS 编译找不到 Qt
- 确保通过 CMAKE_PREFIX_PATH 指向 Qt 安装目录
- 确保 Qt 编译时使用 -extprefix 安装到了正确位置

### HAP 启动白屏
- 检查 entry/libs/arm64-v8a/ 下所有 .so 是否齐全
- 使用 hilog 查看日志: `hdc hilog | grep LMMS`
- 检查 QtAppConstants.ets 中 APP_LIBRARY_NAME 是否正确

### 音频无输出 (Phase 2)
- 确认设备支持 OHAudio API
- 检查应用是否获得音频权限
- 查看 AudioOhAudio 相关日志
