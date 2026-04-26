# LMMS for HarmonyOS 移植编码任务列表

> 基于 spec.md 需求规格和 design.md 实现方案生成
> Phase 1 骨架代码已创建，后续任务需完善骨架实现并推进 Phase 2-5

---

## 1. Phase 1: Qt for HarmonyOS 环境搭建与 GUI 基本显示（骨架已创建，需完善）

### 1.1 CMake 构建系统适配

- [x] T1.1.1 创建 HarmonyOS NDK 交叉编译工具链配置 `lmms-master/cmake/ohos-toolchain.cmake`
- [x] T1.1.2 添加 LMMS_BUILD_OHOS 平台检测 `lmms-master/cmake/modules/DetectMachine.cmake`
- [x] T1.1.3 根 CMakeLists.txt 添加 WANT_OHAUDIO/WANT_OH_MIDI 构建选项及 OHOS 平台分支逻辑
- [x] T1.1.4 配置头文件添加 LMMS_BUILD_OHOS/LMMS_HAVE_OHAUDIO/LMMS_HAVE_OH_MIDI 宏 `lmms-master/src/lmmsconfig.h.in`
- [x] T1.1.5 src/core/CMakeLists.txt 添加 OHOS 平台条件编译音频/MIDI/LV2 后端
- [x] T1.1.6 src/gui/CMakeLists.txt 在 OHOS 平台排除 AudioAlsaSetupWidget/Lv2ViewBase
- [x] T1.1.7 src/3rdparty/CMakeLists.txt 在 OHOS 平台排除 weakjack
- [ ] T1.1.8 完善 src/core/CMakeLists.txt 中 OHAudio 库链接配置，确保链接 `ohaudio` 系统库
  - 修改文件: `lmms-master/src/core/CMakeLists.txt`
  - 变更内容: 在 LMMS_BUILD_OHOS 条件分支中添加 `target_link_libraries(lmmscore PRIVATE ohaudio)`
- [ ] T1.1.9 完善 src/core/CMakeLists.txt 中 OH_MIDI 库链接配置，确保链接 HarmonyOS MIDI 系统库
  - 修改文件: `lmms-master/src/core/CMakeLists.txt`
  - 变更内容: 在 LMMS_BUILD_OHOS 条件分支中添加 `target_link_libraries(lmmscore PRIVATE ohmidi)`
- [ ] T1.1.10 配置根 CMakeLists.txt 中 Qt for HarmonyOS 查找路径 (CMAKE_PREFIX_PATH)，当 LMMS_BUILD_OHOS 时设置 Qt 安装前缀
  - 修改文件: `lmms-master/CMakeLists.txt`
  - 变更内容: 添加 `if(LMMS_BUILD_OHOS) list(APPEND CMAKE_PREFIX_PATH "${QT_OHOS_INSTALL_DIR}") endif()`
- [ ] T1.1.11 添加 vcpkg ohos-arm64 triplet 配置文件
  - 创建文件: `lmms-master/cmake/vcpkg-triplets/ohos-arm64.cmake`
  - 变更内容: 设置 VCPKG_TARGET_ARCHITECTURE=arm64, VCPKG_CMAKE_SYSTEM_NAME=OHOS, VCPKG_CROSSCOMPILING=ON 等

### 1.2 HAP 入口设计

- [x] T1.2.1 创建 QtAppConstants.ets 配置 `LMMS_for_HMOS/entry/src/main/ets/common/QtAppConstants.ets`
- [x] T1.2.2 创建 EntryAbility.ets Qt 应用加载逻辑 `LMMS_for_HMOS/entry/src/main/ets/entryability/EntryAbility.ets`
- [x] T1.2.3 创建 Index.ets LMMS 启动页 `LMMS_for_HMOS/entry/src/main/ets/pages/Index.ets`
- [x] T1.2.4 配置 module.json5 文件读写权限
- [x] T1.2.5 配置 string.json LMMS 描述
- [ ] T1.2.6 完善 EntryAbility.ets 中的 Qt 运行时加载逻辑，确保 globalThis.qtAppLibName/qtQpaPlatform/qtPluginPath 正确设置
  - 修改文件: `LMMS_for_HMOS/entry/src/main/ets/entryability/EntryAbility.ets`
  - 变更内容: 验证并完善 Qt 加载常量赋值、dlopen 调用、错误处理逻辑
- [ ] T1.2.7 在 module.json5 中添加 OHAudio 和 MIDI 所需的系统能力声明
  - 修改文件: `LMMS_for_HMOS/entry/src/main/module.json5`
  - 变更内容: 添加 `"ohos.permission.KEEP_BACKGROUND_RUNNING"` 等音频后台播放所需权限

### 1.3 文件系统适配 (OhosFileSystem)

- [ ] T1.3.1 创建 OhosFileSystem 类头文件
  - 创建文件: `lmms-master/src/core/OhosFileSystem.h`
  - 变更内容: 实现 `OhosFileSystem` 类，提供 `appDataDir()`, `appCacheDir()`, `appResourceDir()`, `pluginDir()`, `lmmsDataDir()`, `samplesDir()`, `presetsDir()`, `mapPath()` 静态方法，全部在 `#ifdef LMMS_BUILD_OHOS` 条件编译内
- [ ] T1.3.2 创建 OhosFileSystem 类实现文件
  - 创建文件: `lmms-master/src/core/OhosFileSystem.cpp`
  - 变更内容: 实现鸿蒙沙箱路径映射表：`~/.lmms/` → `/data/storage/el2/base/files/.lmms/`，`/usr/share/lmms/` → `/data/storage/el1/bundle/files/`，`plugins/` → `/data/storage/el1/bundle/libs/arm64/plugins/`；实现 `mapPath()` 路径转换逻辑
- [ ] T1.3.3 将 OhosFileSystem.cpp 加入 src/core/CMakeLists.txt 的 OHOS 条件编译列表
  - 修改文件: `lmms-master/src/core/CMakeLists.txt`
  - 变更内容: 在 LMMS_BUILD_OHOS 分支添加 `OhosFileSystem.cpp`
- [ ] T1.3.4 适配 ConfigManager 使用 OhosFileSystem 路径映射
  - 修改文件: `lmms-master/src/core/ConfigManager.cpp`
  - 变更内容: 在 `#ifdef LMMS_BUILD_OHOS` 条件下，`workingDir()` 返回 `OhosFileSystem::lmmsDataDir()`，`dataDir()` 返回 `OhosFileSystem::appResourceDir()`

### 1.4 第三方依赖交叉编译

- [ ] T1.4.1 编写 libsndfile for HarmonyOS 交叉编译脚本
  - 创建文件: `lmms-master/cmake/build_deps/ohos-build_libsndfile.cmake`
  - 变更内容: 使用 ohos-clang 工具链编译 libsndfile 鸿蒙 ARM64 版本
- [ ] T1.4.2 编写 FFTW3f for HarmonyOS 交叉编译脚本
  - 创建文件: `lmms-master/cmake/build_deps/ohos-build_fftw3f.cmake`
  - 变更内容: 使用 ohos-clang 工具链编译 FFTW3f 鸿蒙 ARM64 版本
- [ ] T1.4.3 编写 libsamplerate for HarmonyOS 交叉编译脚本
  - 创建文件: `lmms-master/cmake/build_deps/ohos-build_libsamplerate.cmake`
  - 变更内容: 使用 ohos-clang 工具链编译 libsamplerate 鸿蒙 ARM64 版本
- [ ] T1.4.4 编写 FluidSynth for HarmonyOS 交叉编译脚本
  - 创建文件: `lmms-master/cmake/build_deps/ohos-build_fluidsynth.cmake`
  - 变更内容: 使用 ohos-clang 工具链编译 FluidSynth 鸿蒙 ARM64 版本（Sf2Player 插件依赖）

### 1.5 HAP 打包脚本

- [ ] T1.5.1 编写 deploy_to_hap.ps1 部署脚本，将编译产物复制到 HAP libs 目录
  - 创建文件: `LMMS_for_HMOS/deploy_to_hap.ps1`
  - 变更内容: 实现自动复制 liblmms.so、Qt 运行时 .so、第三方依赖 .so、plugins/*.so 到 `entry/libs/arm64-v8a/` 目录

### 1.6 Phase 1 集成验证

- [ ] T1.6.1 验证 LMMS 核心代码交叉编译通过 (CMake + ohos-clang)
- [ ] T1.6.2 验证 LMMS GUI 代码交叉编译通过 (CMake + Qt for HarmonyOS)
- [ ] T1.6.3 验证 DevEco Studio 构建 HAP 成功
- [ ] T1.6.4 验证 HAP 安装到鸿蒙设备后 LMMS MainWindow 显示正常
- [ ] T1.6.5 验证 Phase 1 完成时无音频/MIDI 功能可用

---

## 2. Phase 2: 音频引擎鸿蒙适配（OHAudio API）

### 2.1 AudioOhAudio 核心实现

- [x] T2.1.1 创建 AudioOhAudio 类头文件骨架 `lmms-master/src/core/audio/AudioOhAudio.h`
- [x] T2.1.2 创建 AudioOhAudio 类实现骨架 `lmms-master/src/core/audio/AudioOhAudio.cpp`
- [ ] T2.1.3 完善 AudioOhAudio 构造函数：添加 OH_AudioStreamBuilder_SetBufferDuration 调用
  - 修改文件: `lmms-master/src/core/audio/AudioOhAudio.cpp`
  - 变更内容: 在构造函数中根据缓冲区大小计算 bufferDurationNs = frames * 1e9 / sampleRate，并调用 `OH_AudioStreamBuilder_SetBufferDuration(m_builder, bufferDurationNs)`
- [ ] T2.1.4 完善 audioRendererCallback 回调函数，正确传递 buffer 指针
  - 修改文件: `lmms-master/src/core/audio/AudioOhAudio.cpp`
  - 变更内容: 修改 `audioRendererCallback` 第二个参数（void* userData 之后的 buffer 指针参数），正确获取 OHAudio 提供的输出缓冲区指针并传递给 `processAudioData(buffer, bufferLen)`
- [ ] T2.1.5 实现 processAudioData 核心音频处理算法
  - 修改文件: `lmms-master/src/core/audio/AudioOhAudio.cpp`
  - 变更内容:
    - 计算帧数 `frames = bufferLen / channels`
    - 调用 `mixer()->renderNextBuffer()` 渲染下一缓冲区
    - 获取渲染后的 AudioSampleBuffer（非交错 float32 PCM）
    - 将非交错 PCM 转换为交错 float32 格式写入 OHAudio buffer：`output[frame*2]=input[frame][0]; output[frame*2+1]=input[frame][1]`
    - 缓冲区欠载时零填充并递增 m_bufferUnderrunCount
    - 每 100 次欠载输出一次警告日志
- [ ] T2.1.6 添加 m_bufferUnderrunCount 成员变量及欠载统计逻辑
  - 修改文件: `lmms-master/src/core/audio/AudioOhAudio.h`, `lmms-master/src/core/audio/AudioOhAudio.cpp`
  - 变更内容: 在头文件添加 `uint32_t m_bufferUnderrunCount{0};` 成员；在 processAudioData 中实现欠载统计
- [ ] T2.1.7 实现采样率可配置（支持 44100Hz / 48000Hz）
  - 修改文件: `lmms-master/src/core/audio/AudioOhAudio.cpp`
  - 变更内容: 构造函数中从 ConfigManager 读取用户配置的采样率，而非使用默认值

### 2.2 AudioOhAudioSetupWidget 实现

- [x] T2.2.1 创建 AudioOhAudioSetupWidget 头文件骨架 `lmms-master/src/gui/AudioOhAudioSetupWidget.h`
- [x] T2.2.2 创建 AudioOhAudioSetupWidget 实现骨架 `lmms-master/src/gui/AudioOhAudioSetupWidget.cpp`
- [ ] T2.2.3 添加采样率选择 QComboBox 成员
  - 修改文件: `lmms-master/src/gui/AudioOhAudioSetupWidget.h`, `lmms-master/src/gui/AudioOhAudioSetupWidget.cpp`
  - 变更内容: 添加 `QComboBox* m_sampleRateCombo` 成员，构造函数中初始化并添加 44100/48000 选项，布局到 widget
- [ ] T2.2.4 添加缓冲区大小选择 QComboBox 成员
  - 修改文件: `lmms-master/src/gui/AudioOhAudioSetupWidget.h`, `lmms-master/src/gui/AudioOhAudioSetupWidget.cpp`
  - 变更内容: 添加 `QComboBox* m_bufferSizeCombo` 成员，构造函数中初始化并添加 128/256/512/1024 选项
- [ ] T2.2.5 实现 saveSettings/loadSettings 持久化采样率和缓冲区大小配置
  - 修改文件: `lmms-master/src/gui/AudioOhAudioSetupWidget.cpp`
  - 变更内容: `saveSettings()` 将当前选择写入 ConfigManager；`loadSettings()` 从 ConfigManager 读取并设置到 ComboBox
- [ ] T2.2.6 完善 AudioOhAudio::setupAudioDeviceSetupWidget 注册设置界面
  - 修改文件: `lmms-master/src/core/audio/AudioOhAudio.cpp`
  - 变更内容: 在 `setupAudioDeviceSetupWidget()` 中创建 `AudioOhAudioSetupWidget` 实例并添加到布局

### 2.3 音频设备注册与异常回退

- [ ] T2.3.1 在 AudioEngine 中注册 OHAudio 音频设备工厂
  - 修改文件: `lmms-master/src/core/AudioEngine.cpp`
  - 变更内容: 在 `#ifdef LMMS_HAVE_OHAUDIO` 条件下，注册 AudioOhAudio::instantiate 和 AudioOhAudio::setupAudioDeviceSetupWidget 到音频设备工厂列表
- [ ] T2.3.2 实现 OHAudio 不可用时回退到 DummyAudioDevice 的静音模式
  - 修改文件: `lmms-master/src/core/AudioEngine.cpp`
  - 变更内容: 在音频设备创建逻辑中，当 AudioOhAudio 构造函数 `_successful=false` 时，创建 DummyAudioDevice 并显示"音频设备不可用，应用以静音模式运行"提示
- [ ] T2.3.3 确保鸿蒙平台 ALSA/JACK/PulseAudio/SDL 后端代码不参与编译
  - 修改文件: `lmms-master/CMakeLists.txt`, `lmms-master/src/core/CMakeLists.txt`
  - 变更内容: 验证 LMMS_BUILD_OHOS 条件下 WANT_ALSA=WANT_JACK=WANT_PULSEAUDIO=WANT_SDL=OFF 强制生效

### 2.4 Phase 2 集成验证

- [ ] T2.4.1 验证 AudioOhAudio.cpp 编译通过 (CMake with WANT_OHAUDIO=ON)
- [ ] T2.4.2 验证设置对话框中可选择 OHAudio 音频设备
- [ ] T2.4.3 验证点击播放按钮后扬声器输出音频
- [ ] T2.4.4 验证音频端到端延迟 ≤ 20ms
- [ ] T2.4.5 验证 OHAudio 不可用时自动回退到静音模式
- [ ] T2.4.6 验证鸿蒙构建中 ALSA/JACK/PulseAudio 后端代码不参与编译

---

## 3. Phase 3: MIDI 支持（HarmonyOS MIDI API）

### 3.1 MidiOhMidi 核心实现

- [x] T3.1.1 创建 MidiOhMidi 类头文件骨架 `lmms-master/src/core/midi/MidiOhMidi.h`
- [x] T3.1.2 创建 MidiOhMidi 类实现骨架 `lmms-master/src/core/midi/MidiOhMidi.cpp`
- [ ] T3.1.3 完善 MidiOhMidi.h：添加 HarmonyOS MIDI API 头文件引用和完整成员
  - 修改文件: `lmms-master/src/core/midi/MidiOhMidi.h`
  - 变更内容: 添加 `#include <ohmidi/native_midi_client.h>` 等 HarmonyOS MIDI API 头文件；添加 `OH_MidiClient* m_midiClientHandle`、`QStringList m_inputDevices`、`QStringList m_outputDevices` 成员；添加 `readablePorts()`/`writablePorts()` 重写；添加 `processOutEvent()` 重写；添加 `midiEventCallback()` 静态回调和 `processMidiEvent()`/`mapMidiEvent()`/`enumerateMidiDevices()` 私有方法
- [ ] T3.1.4 实现 MidiOhMidi 构造函数：调用 OH_Midi_EnumerateDevices 和 OH_Midi_CreateClient
  - 修改文件: `lmms-master/src/core/midi/MidiOhMidi.cpp`
  - 变更内容: 构造函数中调用 `OH_Midi_EnumerateDevices()` 枚举设备填充 m_inputDevices/m_outputDevices；调用 `OH_Midi_CreateClient()` 创建客户端句柄；调用 `OH_Midi_RegisterEventCallback()` 注册事件回调
- [ ] T3.1.5 实现 MidiOhMidi 析构函数：释放 MIDI 客户端资源
  - 修改文件: `lmms-master/src/core/midi/MidiOhMidi.cpp`
  - 变更内容: 调用 `OH_Midi_DestroyClient()` 释放 m_midiClientHandle
- [ ] T3.1.6 实现 midiEventCallback 静态回调和 processMidiEvent 事件处理
  - 修改文件: `lmms-master/src/core/midi/MidiOhMidi.cpp`
  - 变更内容: `midiEventCallback()` 将 OH_MidiEvent 传递给 `processMidiEvent()`；`processMidiEvent()` 先经过事件节流检查，通过后调用 `mapMidiEvent()` 转换为 LMMS MidiEvent 并传递给 MidiPort::processInEvent()
- [ ] T3.1.7 实现 mapMidiEvent 事件映射算法
  - 修改文件: `lmms-master/src/core/midi/MidiOhMidi.cpp`
  - 变更内容: 实现 OH_MidiEvent → LMMS MidiEvent 完整映射：NoteOn(0x9)→MidiNoteOn、NoteOff(0x8)→MidiNoteOff、CC(0xB)→MidiControlChange、ProgramChange(0xC)→MidiProgramChange、PitchBend(0xE)→MidiPitchBend（14位合并）、Clock(0xF8)/Start(0xFA)/Stop(0xFC)→MidiMetaEvent；velocity=0 的 NoteOn 视为 NoteOff
- [ ] T3.1.8 实现 processOutEvent 发送 MIDI 事件到外部设备
  - 修改文件: `lmms-master/src/core/midi/MidiOhMidi.cpp`
  - 变更内容: 将 LMMS MidiEvent 转换为 OH_MidiEvent 结构并调用 `OH_Midi_SendEvent()` 发送
- [ ] T3.1.9 实现 readablePorts/writablePorts 设备列表接口
  - 修改文件: `lmms-master/src/core/midi/MidiOhMidi.cpp`
  - 变更内容: `readablePorts()` 返回 m_inputDevices，`writablePorts()` 返回 m_outputDevices
- [ ] T3.1.10 实现 enumerateMidiDevices 设备枚举与热插拔处理
  - 修改文件: `lmms-master/src/core/midi/MidiOhMidi.cpp`
  - 变更内容: 调用 `OH_Midi_EnumerateDevices()` 刷新设备列表；设备连接时发出 readablePortsChanged()/writablePortsChanged() 信号；设备断开时标记为 offline 并通知用户

### 3.2 MIDI 事件节流 (MidiEventThrottle)

- [ ] T3.2.1 创建 MidiEventThrottle 类头文件
  - 创建文件: `lmms-master/src/core/midi/MidiEventThrottle.h`
  - 变更内容: 实现 `MidiEventThrottle` 类，包含 `shouldProcess()`、`reset()` 方法，`m_eventCount`（atomic<uint32_t>）、`m_maxEventsPerSecond`（默认10000）、`m_lastResetTime` 成员
- [ ] T3.2.2 创建 MidiEventThrottle 类实现文件
  - 创建文件: `lmms-master/src/core/midi/MidiEventThrottle.cpp`
  - 变更内容: 实现 `shouldProcess()`：每秒重置计数器，m_eventCount < m_maxEventsPerSecond 时返回 true 并递增，否则返回 false（丢弃）
- [ ] T3.2.3 在 MidiOhMidi 中集成 MidiEventThrottle
  - 修改文件: `lmms-master/src/core/midi/MidiOhMidi.h`, `lmms-master/src/core/midi/MidiOhMidi.cpp`
  - 变更内容: 添加 `MidiEventThrottle m_eventThrottle` 成员；在 processMidiEvent 中调用 `m_eventThrottle.shouldProcess()` 进行节流判断
- [ ] T3.2.4 将 MidiEventThrottle.cpp 加入 src/core/CMakeLists.txt 的 OHOS 条件编译列表
  - 修改文件: `lmms-master/src/core/CMakeLists.txt`

### 3.3 MIDI 后端注册与异常处理

- [ ] T3.3.1 在 MidiClient::openMidiClient 中注册 MidiOhMidi 为鸿蒙平台 MIDI 后端
  - 修改文件: `lmms-master/src/core/midi/MidiClient.cpp`
  - 变更内容: 在 `#ifdef LMMS_HAVE_OH_MIDI` 条件下，创建 MidiOhMidi 实例作为 MIDI 客户端
- [ ] T3.3.2 实现 MIDI API 不可用时优雅降级
  - 修改文件: `lmms-master/src/core/midi/MidiClient.cpp`
  - 变更内容: MidiOhMidi 构造失败时 MIDI 功能禁用但不影响其他功能，MIDI 设置中显示"当前设备不支持MIDI功能"提示
- [ ] T3.3.3 确保鸿蒙平台 ALSA-Seq/JACK-MIDI/OSS/WinMM 后端代码不参与编译
  - 修改文件: `lmms-master/src/core/CMakeLists.txt`
  - 变更内容: 验证 LMMS_BUILD_OHOS 条件下 MIDI Linux/Windows 后端源文件不参与编译

### 3.4 Phase 3 集成验证

- [ ] T3.4.1 验证 MidiOhMidi.cpp 编译通过 (CMake with WANT_OH_MIDI=ON)
- [ ] T3.4.2 验证连接 MIDI 设备后设置中显示设备列表
- [ ] T3.4.3 验证外部 MIDI 键盘 NoteOn 触发 LMMS 音符发声
- [ ] T3.4.4 验证 MIDI 事件到音频输出延迟 ≤ 5ms
- [ ] T3.4.5 验证密集 MIDI 事件时节流机制正常工作（丢弃超量事件，不崩溃）
- [ ] T3.4.6 验证 MIDI 设备物理断开后应用不崩溃并通知用户

---

## 4. Phase 4: 插件系统适配

### 4.1 OhosPluginLoader 实现

- [ ] T4.1.1 创建 OhosPluginLoader 类头文件
  - 创建文件: `lmms-master/src/core/OhosPluginLoader.h`
  - 变更内容: 实现 `OhosPluginLoader` 类，包含 `setPluginDir()`、`discover()`、`load()`、`unload()` 方法；`m_pluginDir`（QString）、`m_loadedLibs`（QMap<QString, void*>）成员
- [ ] T4.1.2 创建 OhosPluginLoader 类实现文件
  - 创建文件: `lmms-master/src/core/OhosPluginLoader.cpp`
  - 变更内容: `discover()` 遍历 m_pluginDir 下所有 lib*.so 文件，逐个 dlopen 加载并解析 `lmms_plugin_main` 符号获取 Plugin::Descriptor；`load()` 加载指定插件；`unload()` 调用 dlclose 卸载；默认 pluginDir 为 `/data/storage/el1/bundle/libs/arm64/plugins/`
- [ ] T4.1.3 将 OhosPluginLoader 集成到 PluginFactory
  - 修改文件: `lmms-master/src/core/PluginFactory.cpp`
  - 变更内容: 在 `#ifdef LMMS_BUILD_OHOS` 条件下，使用 OhosPluginLoader 替代默认的插件发现逻辑；调用 `OhosPluginLoader::discover()` 扫描插件
- [ ] T4.1.4 将 OhosPluginLoader.cpp 加入 src/core/CMakeLists.txt 的 OHOS 条件编译列表
  - 修改文件: `lmms-master/src/core/CMakeLists.txt`

### 4.2 插件路径适配

- [ ] T4.2.1 修改 LMMS 插件搜索路径指向 HAP 内 plugins 目录
  - 修改文件: `lmms-master/src/core/PluginFactory.cpp`
  - 变更内容: 在 `#ifdef LMMS_BUILD_OHOS` 条件下，设置插件搜索路径为 `OhosFileSystem::pluginDir()`（即 `/data/storage/el1/bundle/libs/arm64/plugins/`）
- [ ] T4.2.2 修改 plugins/ 各子目录 CMakeLists.txt，在 OHOS 平台设置正确的库输出路径
  - 修改文件: `lmms-master/plugins/*/CMakeLists.txt` (各插件目录)
  - 变更内容: 在 LMMS_BUILD_OHOS 条件下设置 LIBRARY_OUTPUT_DIRECTORY 指向 plugins 子目录

### 4.3 核心插件编译适配

- [ ] T4.3.1 适配核心 Instrument 插件编译：TripleOscillator, Kicker, Organic, Sfxr
  - 修改文件: `lmms-master/plugins/triple_oscillator/CMakeLists.txt`, `lmms-master/plugins/kicker/CMakeLists.txt`, `lmms-master/plugins/organic/CMakeLists.txt`, `lmms-master/plugins/sfxr/CMakeLists.txt`
  - 变更内容: 确保各插件 CMakeLists.txt 支持 ohos-clang 交叉编译，链接 lmmscore/lmms_gui
- [ ] T4.3.2 适配高级 Instrument 插件编译：Monstro, Nes, Sid, Lb302, OpulenZ, Watsyn, Xpressive, Vibed, Patman
  - 修改文件: 对应各插件目录 CMakeLists.txt
  - 变更内容: 确保各插件 CMakeLists.txt 支持 ohos-clang 交叉编译
- [ ] T4.3.3 适配 SF2/GIG 播放插件：Sf2Player, GigPlayer
  - 修改文件: `lmms-master/plugins/sf2_player/CMakeLists.txt`, `lmms-master/plugins/gig_player/CMakeLists.txt`
  - 变更内容: 确保 FluidSynth/libgig 依赖在 OHOS 平台正确链接
- [ ] T4.3.4 适配核心 Effect 插件编译：Amplifier, BassBooster, Bitcrush, Delay, Flanger, ReverbSC
  - 修改文件: 对应各插件目录 CMakeLists.txt
  - 变更内容: 确保各插件 CMakeLists.txt 支持 ohos-clang 交叉编译
- [ ] T4.3.5 适配高级 Effect 插件编译：Compressor, StereoEnhancer, Eq, MultitapEcho, StkEffects
  - 修改文件: 对应各插件目录 CMakeLists.txt
  - 变更内容: 确保各插件 CMakeLists.txt 支持 ohos-clang 交叉编译，处理可选依赖（STK）
- [ ] T4.3.6 适配 LADSPA 内嵌效果器：Calf, Caps, CMT, SWH, TAP
  - 修改文件: `lmms-master/plugins/ladspa/` 下各子目录 CMakeLists.txt
  - 变更内容: 确保 LADSPA SDK 在 OHOS 平台正确编译和链接
- [ ] T4.3.7 适配 Tool 插件：MidiImport, MidiExport, HydrogenImport, SlicerT
  - 修改文件: 对应各插件目录 CMakeLists.txt
  - 变更内容: 确保各插件 CMakeLists.txt 支持 ohos-clang 交叉编译

### 4.4 LV2/Carla/VST 排除

- [ ] T4.4.1 确保鸿蒙构建配置中 LV2/Carla/VST 相关代码不参与编译
  - 修改文件: `lmms-master/CMakeLists.txt`
  - 变更内容: 验证 LMMS_BUILD_OHOS 条件下 WANT_LV2=WANT_CARLA=WANT_VST=OFF 强制生效
- [ ] T4.4.2 实现加载 LV2/Carla/VST 插件时显示"暂不支持该插件类型"提示
  - 修改文件: `lmms-master/src/core/PluginFactory.cpp`
  - 变更内容: 在 `#ifdef LMMS_BUILD_OHOS` 条件下，检测到 LV2/Carla/VST 插件类型时返回友好提示而非崩溃

### 4.5 插件崩溃隔离

- [ ] T4.5.1 实现插件崩溃信号处理（SIGSEGV/SIGABRT）
  - 创建文件: `lmms-master/src/core/PluginCrashGuard.h`, `lmms-master/src/core/PluginCrashGuard.cpp`
  - 变更内容: 注册 SIGSEGV/SIGABRT 信号处理器；在信号处理器中识别崩溃地址所属插件共享库，卸载该插件（dlclose），从 PluginFactory 移除，恢复 AudioEngine 运行，显示"插件 [名称] 运行出错已卸载"通知
- [ ] T4.5.2 将 PluginCrashGuard.cpp 加入 src/core/CMakeLists.txt 的 OHOS 条件编译列表
  - 修改文件: `lmms-master/src/core/CMakeLists.txt`

### 4.6 Phase 4 集成验证

- [ ] T4.6.1 验证所有目标插件鸿蒙 ARM64 版本编译成功
- [ ] T4.6.2 验证启动时自动加载 HAP 内 plugins 目录下所有插件
- [ ] T4.6.3 验证 TripleOscillator 插件界面正确显示，旋钮可操作
- [ ] T4.6.4 验证 Amplifier 效果器正确处理音频信号
- [ ] T4.6.5 验证插件 .so 损坏时跳过该插件且不崩溃
- [ ] T4.6.6 验证鸿蒙版不包含 LV2/Carla/VST 相关编译代码

---

## 5. Phase 5: 完整功能测试与优化

### 5.1 项目文件操作验证

- [ ] T5.1.1 验证 .mmp 格式项目文件完整读写（打开→编辑→保存→重新打开，数据一致）
- [ ] T5.1.2 验证 .mmpz 格式项目文件完整读写（gzip 压缩格式）
- [ ] T5.1.3 验证打开 LMMS 1.2.0 创建的项目文件向后兼容
- [ ] T5.1.4 验证损坏 XML 项目文件恢复可读部分并警告用户
- [ ] T5.1.5 验证配置持久化：修改设置后重启应用配置保持

### 5.2 音频导出验证

- [ ] T5.2.1 验证导出 WAV 44100Hz 16bit 文件可正常播放
- [ ] T5.2.2 验证导出 FLAC 格式文件可正常播放
- [ ] T5.2.3 验证导出 OGG 格式文件可正常播放
- [ ] T5.2.4 验证导出 MP3 格式文件可正常播放
- [ ] T5.2.5 验证磁盘空间不足时导出停止并提示用户

### 5.3 编辑器功能验证

- [ ] T5.3.1 验证 PianoRoll 音符绘制/选择/移动/缩放/量化功能正常
- [ ] T5.3.2 验证 AutomationEditor 自动化曲线编辑/节点操作功能正常
- [ ] T5.3.3 验证 SongEditor 轨道显示/Clip 拖拽/缩放/滚动正常
- [ ] T5.3.4 验证 BBEditor (Beat/Bassline Editor) 功能正常

### 5.4 自定义控件验证

- [ ] T5.4.1 验证 Knob 控件渲染和交互正常（鼠标/触摸拖拽旋转）
- [ ] T5.4.2 验证 Fader 控件渲染和交互正常（垂直拖拽）
- [ ] T5.4.3 验证 Graph 控件渲染和交互正常（曲线编辑）
- [ ] T5.4.4 验证 ComboBox/LedCheckbox/TabWidget/GroupBox 等基础控件正常
- [ ] T5.4.5 验证 PixmapButton/TextFloat/FadeButton/Meter/LcdWidget 等高级控件正常
- [ ] T5.4.6 验证全部 31 个自定义控件逐一渲染和交互正常

### 5.5 性能优化

- [ ] T5.5.1 启动时间优化：实现非核心插件后台线程扫描（延迟加载）
  - 修改文件: `lmms-master/src/core/PluginFactory.cpp`
  - 变更内容: 将插件扫描拆分为核心插件（前台）和非核心插件（后台线程），减少启动阻塞
- [ ] T5.5.2 启动时间优化：资源预编译，确保 .qrc 资源编译到二进制
  - 修改文件: `lmms-master/CMakeLists.txt`, `lmms-master/src/gui/CMakeLists.txt`
  - 变更内容: 确保 qt5_add_resources 将所有 .qrc 资源编译进二进制，避免运行时文件读取
- [ ] T5.5.3 音频延迟优化：验证 AUDIOSTREAM_LATENCY_MODE_LOW 模式生效，缓冲区默认 256 帧
  - 修改文件: `lmms-master/src/core/audio/AudioOhAudio.cpp`
  - 变更内容: 验证并确保 OH_AudioStreamBuilder_SetLatencyMode 使用 LOW 模式；高级用户可调至 128 帧
- [ ] T5.5.4 内存占用优化：验证空闲状态 ≤ 200MB，10 轨道典型项目 ≤ 500MB
- [ ] T5.5.5 验证冷启动到主窗口可操作时间 ≤ 5 秒
- [ ] T5.5.6 验证 GUI 渲染帧率 ≥ 30fps

### 5.6 触摸交互适配验证

- [ ] T5.6.1 验证鸿蒙触摸事件通过 libqtaf_ohos.so 正确映射为 QMouseEvent
- [ ] T5.6.2 验证 Knob/Fader 触摸拖拽操作响应正常
- [ ] T5.6.3 验证 PianoRoll/SongEditor 触摸交互操作响应正常

### 5.7 发布准备

- [ ] T5.7.1 验证 HAP 包在鸿蒙设备上完整安装和卸载
- [ ] T5.7.2 编写应用版本信息显示（LMMS 上游版本号 + 鸿蒙移植版本号）
  - 修改文件: `lmms-master/src/gui/dialogs/AboutDialog.cpp`
  - 变更内容: 在 `#ifdef LMMS_BUILD_OHOS` 条件下，AboutDialog 显示 "LMMS 1.3.0-alpha (HarmonyOS Port)" 版本信息
- [ ] T5.7.3 更新 other/PLAN.md 计划列表，标记所有 Phase 为已完成
- [ ] T5.7.4 更新 other/BUILD_GUIDE.md 构建指南，补充完整的构建步骤说明
