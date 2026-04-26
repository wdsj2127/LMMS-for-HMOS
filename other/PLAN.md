# LMMS for HarmonyOS - 移植计划列表

> 将LMMS 1.3.0-alpha移植到鸿蒙OS，基于Qt 5.15.16 for HarmonyOS技术方案

## Phase 1: Qt for HarmonyOS环境搭建与GUI基本显示

### 1.1 Qt for HarmonyOS交叉编译环境配置
- [ ] 获取Qt 5.15.16 for HarmonyOS源码（Qt Gerrit tqtc-qt5 tqtc/harmonyos-5.15.16分支）
- [ ] 配置HarmonyOS NDK环境变量（NATIVE_OHOS_SDK, OHOS_SDK_SYSROOT, LLVM_INSTALL_DIR）
- [ ] 交叉编译Qt框架：`configure -xplatform ohos-clang -ohos-arch arm64-v8a` → `make` → `make install`
- [ ] 验证：生成libQt5Core.so, libQt5Gui.so, libQt5Widgets.so, libQt5Xml.so, libQt5Svg.so
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 无

### 1.2 LMMS CMake构建系统适配ohos-clang
- [ ] 创建CMake工具链文件 `cmake/ohos-toolchain.cmake`（指定ohos-clang编译器、sysroot、API Level）
- [ ] 修改根CMakeLists.txt添加HarmonyOS平台检测（`LMMS_BUILD_OHOS`选项）
- [ ] 在HarmonyOS平台下禁用Linux音频后端（ALSA/JACK/OSS/PulseAudio/SDL）
- [ ] 在HarmonyOS平台下禁用Linux MIDI后端（ALSA-Seq/JACK-MIDI/OSS）
- [ ] 在HarmonyOS平台下禁用X11相关代码（qt5-x11embed）
- [ ] 配置Qt for HarmonyOS的查找路径（CMAKE_PREFIX_PATH指向Qt安装目录）
- [ ] 验证：CMake configure成功，无Linux平台专属依赖
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 1.1

### 1.3 DevEco HAP项目配置Qt应用入口
- [ ] 修改EntryAbility.ets，配置Qt应用加载逻辑（参考Qt for HarmonyOS模板项目）
- [ ] 创建 `entry/src/main/ets/common/QtAppConstants.ets`，定义APP_LIBRARY_NAME为liblmms.so
- [ ] 在entry/libs/arm64-v8a/目录下创建Qt运行时库和LMMS编译产物的存放结构
- [ ] 配置module.json5添加必要的权限（文件读写、音频、MIDI）
- [ ] 配置build-profile.json5确保API Level 15+兼容
- [ ] 验证：DevEco Studio可构建HAP包
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 1.2

### 1.4 LMMS核心代码平台兼容性适配
- [ ] 审计src/core/中所有平台相关代码（#ifdef LMMS_BUILD_OHOS条件编译）
- [ ] 适配AudioDevice抽象层：创建AudioOhAudio类骨架（Phase 2填充实现）
- [ ] 适配MidiClient抽象层：创建MidiOhMidi类骨架（Phase 3填充实现）
- [ ] 适配文件路径：鸿蒙应用私有目录路径映射
- [ ] 适配Plugin加载：鸿蒙HAP内部路径的dlopen加载
- [ ] 适配Process/Thread：鸿蒙NDK的pthread支持确认
- [ ] 适配SampleDecoder：确认libsndfile鸿蒙版可用
- [ ] 验证：src/core/在ohos-clang下编译通过
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 1.2

### 1.5 LMMS GUI代码平台兼容性适配
- [ ] 确认Qt Widgets在Qt for HarmonyOS下的可用性
- [ ] 适配src/gui/中所有QWidget子类的paintEvent/resizeEvent等事件处理
- [ ] 适配31个自定义控件（Knob, Fader, Graph, ComboBox, LedCheckbox等）
- [ ] 适配MainWindow：QMdiArea在鸿蒙平台的行为
- [ ] 适配嵌入资源（.qrc文件）：确保Qt资源系统正常工作
- [ ] 适配LmmsStyle/LmmsPalette：确认Qt样式表在鸿蒙渲染正常
- [ ] 适配触摸事件映射：将HarmonyOS触摸事件映射为QMouseEvent
- [ ] 验证：src/gui/在Qt for HarmonyOS下编译通过
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 1.1, 1.4

### 1.6 第三方依赖库鸿蒙版编译
- [ ] 配置vcpkg triplet for HarmonyOS（ohos-arm64）
- [ ] 交叉编译libsndfile for HarmonyOS ARM64
- [ ] 交叉编译FFTW3f for HarmonyOS ARM64
- [ ] 交叉编译libsamplerate for HarmonyOS ARM64
- [ ] 交叉编译FluidSynth for HarmonyOS ARM64（Sf2Player插件依赖）
- [ ] 交叉编译STK for HarmonyOS ARM64（可选）
- [ ] 交叉编译LADSPA SDK for HarmonyOS ARM64
- [ ] 验证：所有第三方库生成鸿蒙ARM64版.a/.so文件
- **状态**: 待开始 | **优先级**: 中 | **依赖**: 1.1

### 1.7 Phase 1集成验证
- [ ] 完整编译LMMS for HarmonyOS（core + gui + 依赖库）
- [ ] 将编译产物和Qt运行时库放入entry/libs/arm64-v8a/
- [ ] 构建HAP包并安装到鸿蒙设备
- [ ] 验证：启动后显示LMMS MainWindow界面（SongEditor/工具栏/侧边栏）
- [ ] 验证：无音频/MIDI功能可用（符合Phase 1范围）
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 1.3, 1.4, 1.5, 1.6

---

## Phase 2: 音频引擎鸿蒙适配（OHAudio API）

### 2.1 实现AudioOhAudio类
- [ ] 创建src/core/audio/AudioOhAudio.h / AudioOhAudio.cpp
- [ ] 继承AudioDevice基类，实现writeBuffer/startProcessing/stopProcessing
- [ ] 使用OHAudio API创建音频输出流（OH_AudioStream_Create）
- [ ] 配置音频流参数（float32 PCM, 44100/48000Hz, stereo）
- [ ] 实现音频回调函数：OHAudio请求PCM数据时调用Mixer::renderNextBuffer()
- [ ] 验证：编译通过，类接口正确
- **状态**: 待开始 | **优先级**: 高 | **依赖**: Phase 1

### 2.2 集成AudioOhAudio到构建系统
- [ ] CMakeLists.txt添加WANT_OHAUDIO选项
- [ ] 链接OHAudio库
- [ ] 配置AudioDeviceSetupWidget中添加OHAudio选项
- [ ] 验证：构建成功，设置中可选OHAudio
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 2.1

### 2.3 音频输出功能验证
- [ ] 创建简单测试项目（单轨道单音符）
- [ ] 点击播放验证扬声器输出音频
- [ ] 验证音频延迟≤20ms
- [ ] 验证OHAudio不可用时回退到静音模式
- [ ] 验证音频缓冲区欠载时输出零填充数据
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 2.2

---

## Phase 3: MIDI支持（HarmonyOS MIDI API）

### 3.1 实现MidiOhMidi类
- [ ] 创建src/core/midi/MidiOhMidi.h / MidiOhMidi.cpp
- [ ] 继承MidiClient基类，实现HarmonyOS MIDI API事件收发
- [ ] 实现MIDI设备枚举
- [ ] 实现MIDI事件映射（HarmonyOS → LMMS MidiEvent）
- [ ] 实现MIDI时钟同步
- [ ] 验证：编译通过，类接口正确
- **状态**: 待开始 | **优先级**: 高 | **依赖**: Phase 1

### 3.2 集成MidiOhMidi到构建系统
- [ ] CMakeLists.txt添加WANT_OH_MIDI选项
- [ ] 链接HarmonyOS MIDI库
- [ ] 配置MIDI设置UI
- [ ] 验证：构建成功
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 3.1

### 3.3 MIDI功能验证
- [ ] 连接外部MIDI键盘验证NoteOn/NoteOff响应
- [ ] 验证MIDI事件延迟≤5ms
- [ ] 验证MIDI设备断开时应用不崩溃
- [ ] 验证MIDI API不可用时功能优雅降级
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 3.2

---

## Phase 4: 插件系统适配

### 4.1 内嵌插件交叉编译
- [ ] 编译核心Instrument插件：TripleOscillator, Kicker, Organic, Sfxr, Monstro, Nes, Sid, Lb302, OpulenZ, Watsyn, Xpressive, Vibed, Patman
- [ ] 编译核心Effect插件：Amplifier, BassBooster, Bitcrush, BitInvader, Compressor, Delay, Flanger, ReverbSC, StereoEnhancer, Eq
- [ ] 编译Sf2Player（需FluidSynth）, GigPlayer, SlicerT
- [ ] 编译LADSPA内嵌效果器
- [ ] 编译MidiImport, MidiExport, HydrogenImport
- [ ] 验证：所有插件生成鸿蒙ARM64版.so
- **状态**: 待开始 | **优先级**: 高 | **依赖**: Phase 1

### 4.2 插件加载路径适配
- [ ] 修改PluginFactory在鸿蒙平台的插件搜索路径指向HAP内部plugins目录
- [ ] 配置HAP打包脚本将插件.so放入正确路径
- [ ] 实现插件崩溃隔离（信号捕获/插件卸载）
- [ ] 验证：启动时自动加载所有内嵌插件
- **状态**: 待开始 | **优先级**: 高 | **依赖**: 4.1

### 4.3 插件GUI验证
- [ ] 验证TripleOscillator界面显示和旋钮操作
- [ ] 验证Amplifier等Effect插件界面
- [ ] 验证Sf2Player加载SF2音色库
- [ ] 验证LV2/Carla/VST提示"暂不支持"
- **状态**: 待开始 | **优先级**: 中 | **依赖**: 4.2

---

## Phase 5: 完整功能测试与优化

### 5.1 项目文件功能验证
- [ ] 打开/保存.mmp和.mmpz文件
- [ ] 打开LMMS 1.2.0创建的项目文件
- [ ] 损坏项目文件的恢复测试
- **状态**: 待开始 | **优先级**: 高 | **依赖**: Phase 2, 3, 4

### 5.2 编辑器功能验证
- [ ] PianoRoll音符绘制/选择/移动/缩放/量化
- [ ] AutomationEditor曲线编辑/节点操作
- [ ] SongEditor轨道显示/Clip拖拽/缩放/滚动
- [ ] 31个自定义控件渲染和交互
- **状态**: 待开始 | **优先级**: 高 | **依赖**: Phase 2, 3, 4

### 5.3 音频导出功能验证
- [ ] 导出WAV/FLAC/OGG/MP3格式
- [ ] 验证导出文件可正常播放
- **状态**: 待开始 | **优先级**: 中 | **依赖**: Phase 2

### 5.4 性能优化
- [ ] 冷启动时间优化（目标≤5秒）
- [ ] 内存占用优化（空闲≤200MB，10轨道≤500MB）
- [ ] GUI帧率优化（目标≥30fps）
- [ ] 音频延迟优化（目标≤20ms）
- **状态**: 待开始 | **优先级**: 中 | **依赖**: 5.1, 5.2

### 5.5 配置持久化与发布准备
- [ ] 配置持久化到鸿蒙应用私有目录
- [ ] 应用签名与发布配置
- [ ] 最终HAP包构建与验证
- **状态**: 待开始 | **优先级**: 中 | **依赖**: 5.4

---

## Phase依赖关系

```
Phase 1 (环境搭建+GUI显示)
    ├── Phase 2 (OHAudio音频适配) ──┐
    ├── Phase 3 (MIDI适配)         ├── Phase 5 (完整测试与优化)
    └── Phase 4 (插件系统适配)  ──┘
```

## 关键技术参考

- Qt for HarmonyOS Wiki: https://wiki.qt.io/Qt_for_HarmonyOS
- Qt for HarmonyOS构建指南: https://wiki.qt.io/Building_Qt_for_HarmonyOS
- Qt for HarmonyOS用户开发指南: https://wiki.qt.io/Qt_for_HarmonyOS/user_development_guide
- Qt源码: https://codereview.qt-project.org/admin/repos/qt/tqtc-qt5 (分支: tqtc/harmonyos-5.15.16)
- DevEco Studio: https://developer.huawei.com/consumer/cn/deveco-studio/
