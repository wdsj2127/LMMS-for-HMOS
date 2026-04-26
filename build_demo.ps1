param(
    [string]$OhosSdkRoot = "",
    [string]$QtOhosDir = "",
    [string]$VcpkgRoot = "",
    [switch]$SkipQt,
    [switch]$SkipDeps,
    [switch]$SkipLmms,
    [switch]$SkipHap
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition
$ProjectRoot = Split-Path -Parent $ProjectRoot
$LmmsSrc = Join-Path $ProjectRoot "lmms-master"
$HapProject = Join-Path $ProjectRoot "LMMS_for_HMOS"
$BuildDir = Join-Path $ProjectRoot "build_lmms_ohos"

Write-Host "================================================" -ForegroundColor Cyan
Write-Host "  LMMS for HarmonyOS - Demo Cross-Compile Build" -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan
Write-Host ""

# Auto-detect OHOS SDK
if ($OhosSdkRoot -eq "") {
    $defaultPaths = @(
        "D:\oh\DevEcoStudio\sdk\HarmonyOS-NEXT-DB6\openharmony\native",
        "D:\DevEcoStudio\sdk\HarmonyOS-NEXT-DB6\openharmony\native",
        "$env:LOCALAPPDATA\Huawei\Sdk\openharmony\native"
    )
    foreach ($p in $defaultPaths) {
        if (Test-Path $p) {
            $OhosSdkRoot = $p
            break
        }
    }
    if ($OhosSdkRoot -eq "") {
        Write-Host "ERROR: OHOS SDK not found. Set -OhosSdkRoot or NATIVE_OHOS_SDK env var." -ForegroundColor Red
        exit 1
    }
}

$env:NATIVE_OHOS_SDK = $OhosSdkRoot
$env:OHOS_SDK_SYSROOT = Join-Path $OhosSdkRoot "sysroot"
$env:LLVM_INSTALL_DIR = Join-Path $OhosSdkRoot "llvm"

Write-Host "OHOS SDK:      $OhosSdkRoot" -ForegroundColor Green
Write-Host "LLVM:          $env:LLVM_INSTALL_DIR" -ForegroundColor Green
Write-Host "LMMS Source:   $LmmsSrc" -ForegroundColor Green
Write-Host "Build Dir:     $BuildDir" -ForegroundColor Green
Write-Host "HAP Project:   $HapProject" -ForegroundColor Green
Write-Host ""

# ============================================================
# Step 1: Build Qt for HarmonyOS
# ============================================================
if (-not $SkipQt) {
    if ($QtOhosDir -eq "") {
        $QtOhosDir = "C:\Qt\qt-5.15.16-ohos"
    }

    if (Test-Path (Join-Path $QtOhosDir "lib\libQt5Core.so")) {
        Write-Host "[SKIP] Qt for HarmonyOS already built at: $QtOhosDir" -ForegroundColor Yellow
    } else {
        Write-Host "[STEP 1] Building Qt 5.15.16 for HarmonyOS..." -ForegroundColor Cyan
        Write-Host "  This requires Qt source code with tqtc/harmonyos-5.15.16 branch." -ForegroundColor Yellow
        Write-Host "  Please run this step manually. See other/BUILD_GUIDE.md for instructions." -ForegroundColor Yellow
        Write-Host ""
        Write-Host "  Quick steps:" -ForegroundColor Yellow
        Write-Host "    1. git clone https://codereview.qt-project.org/qt/tqtc-qt5" -ForegroundColor Yellow
        Write-Host "    2. cd tqtc-qt5 && git checkout tqtc/harmonyos-5.15.16" -ForegroundColor Yellow
        Write-Host "    3. git submodule update --init --recursive" -ForegroundColor Yellow
        Write-Host "    4. mkdir build && cd build" -ForegroundColor Yellow
        Write-Host "    5. ..\configure -xplatform ohos-clang -ohos-arch arm64-v8a -prefix /data/storage/el1/bundle/libs/arm64 -extprefix $QtOhosDir -opensource -confirm-license -release -nomake tests -nomake examples" -ForegroundColor Yellow
        Write-Host "    6. mingw32-make -j8 && mingw32-make install" -ForegroundColor Yellow
        Write-Host ""
        Write-Host "ERROR: Qt not found at $QtOhosDir. Build Qt first or set -QtOhosDir." -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "[SKIP] Qt build skipped by -SkipQt flag" -ForegroundColor Yellow
    if ($QtOhosDir -eq "") { $QtOhosDir = "C:\Qt\qt-5.15.16-ohos" }
}

Write-Host "Qt for OHOS:   $QtOhosDir" -ForegroundColor Green
Write-Host ""

# ============================================================
# Step 2: Cross-compile third-party dependencies
# ============================================================
if (-not $SkipDeps) {
    Write-Host "[STEP 2] Cross-compiling third-party dependencies..." -ForegroundColor Cyan

    if ($VcpkgRoot -eq "") {
        $VcpkgRoot = Join-Path $ProjectRoot "vcpkg"
    }

    if (-not (Test-Path $VcpkgRoot)) {
        Write-Host "  Cloning vcpkg..." -ForegroundColor Yellow
        git clone https://github.com/microsoft/vcpkg.git $VcpkgRoot
        Push-Location $VcpkgRoot
        .\bootstrap-vcpkg.bat
        Pop-Location
    }

    $tripletFile = Join-Path $VcpkgRoot "triplets\ohos-arm64.cmake"
    $srcTriplet = Join-Path $LmmsSrc "cmake\vcpkg-triplets\ohos-arm64.cmake"
    if (-not (Test-Path $tripletFile)) {
        Copy-Item $srcTriplet $tripletFile
    }

    Write-Host "  Installing dependencies via vcpkg..." -ForegroundColor Yellow
    & (Join-Path $VcpkgRoot "vcpkg") install `
        libsndfile:ohos-arm64 `
        fftw3:ohos-arm64 `
        libsamplerate:ohos-arm64 `
        libflac:ohos-arm64 `
        libvorbis:ohos-arm64 `
        libogg:ohos-arm64 `
        --overlay-triplets=(Join-Path $VcpkgRoot "triplets")

    if ($LASTEXITCODE -ne 0) {
        Write-Host "WARNING: vcpkg install failed. You may need to build deps manually." -ForegroundColor Yellow
    }
} else {
    Write-Host "[SKIP] Dependencies build skipped by -SkipDeps flag" -ForegroundColor Yellow
}

Write-Host ""

# ============================================================
# Step 3: Cross-compile LMMS
# ============================================================
if (-not $SkipLmms) {
    Write-Host "[STEP 3] Cross-compiling LMMS for HarmonyOS..." -ForegroundColor Cyan

    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }
    New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

    $cmakeArgs = @(
        "-S", $LmmsSrc,
        "-B", $BuildDir,
        "-DCMAKE_TOOLCHAIN_FILE=$LmmsSrc\cmake\ohos-toolchain.cmake",
        "-DLMMS_BUILD_OHOS=ON",
        "-DQT_OHOS_INSTALL_DIR=$QtOhosDir",
        "-DCMAKE_PREFIX_PATH=$QtOhosDir",
        "-DWANT_ALSA=OFF",
        "-DWANT_OSS=OFF",
        "-DWANT_PULSEAUDIO=OFF",
        "-DWANT_SDL=OFF",
        "-DWANT_JACK=OFF",
        "-DWANT_LV2=OFF",
        "-DWANT_CARLA=OFF",
        "-DWANT_VST=OFF",
        "-DWANT_PORTAUDIO=OFF",
        "-DWANT_SOUNDIO=OFF",
        "-DWANT_SNDIO=OFF",
        "-DWANT_OHAUDIO=ON",
        "-DWANT_OH_MIDI=ON",
        "-DWANT_SF2=ON",
        "-DWANT_GIG=OFF",
        "-DWANT_STK=OFF",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_FIND_ROOT_PATH=$OhosSdkRoot\sysroot"
    )

    Write-Host "  Running CMake configure..." -ForegroundColor Yellow
    Write-Host "  Args: $cmakeArgs" -ForegroundColor DarkGray
    & cmake $cmakeArgs

    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: CMake configure failed." -ForegroundColor Red
        Write-Host ""
        Write-Host "Common fixes:" -ForegroundColor Yellow
        Write-Host "  - Ensure Qt for HarmonyOS is built and installed at $QtOhosDir" -ForegroundColor Yellow
        Write-Host "  - Ensure NATIVE_OHOS_SDK points to correct NDK path" -ForegroundColor Yellow
        Write-Host "  - Check that libQt5Core.so exists in $QtOhosDir\lib\" -ForegroundColor Yellow
        exit 1
    }

    Write-Host "  Building LMMS..." -ForegroundColor Yellow
    & cmake --build $BuildDir --config Release -j 8

    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: LMMS build failed. Check compile errors above." -ForegroundColor Red
        exit 1
    }

    Write-Host "  LMMS build successful!" -ForegroundColor Green
} else {
    Write-Host "[SKIP] LMMS build skipped by -SkipLmms flag" -ForegroundColor Yellow
}

Write-Host ""

# ============================================================
# Step 4: Deploy to HAP and build HAP
# ============================================================
if (-not $SkipHap) {
    Write-Host "[STEP 4] Deploying to HAP project..." -ForegroundColor Cyan

    $deployScript = Join-Path $HapProject "deploy_to_hap.ps1"
    if (Test-Path $deployScript) {
        & powershell -ExecutionPolicy Bypass -File $deployScript `
            -LmmsBuildDir $BuildDir `
            -QtInstallDir $QtOhosDir `
            -HapProjectDir $HapProject
    } else {
        Write-Host "WARNING: deploy_to_hap.ps1 not found. Copy files manually." -ForegroundColor Yellow
    }

    Write-Host ""
    Write-Host "HAP project ready at: $HapProject" -ForegroundColor Green
    Write-Host ""
    Write-Host "Next steps:" -ForegroundColor Cyan
    Write-Host "  1. Open DevEco Studio" -ForegroundColor White
    Write-Host "  2. File -> Open -> select $HapProject" -ForegroundColor White
    Write-Host "  3. Configure signing: File -> Project Structure -> Signing Configs" -ForegroundColor White
    Write-Host "  4. Build > Build Hap(s)/APP(s) > Build APP" -ForegroundColor White
    Write-Host "  5. Run on HarmonyOS device" -ForegroundColor White
} else {
    Write-Host "[SKIP] HAP deployment skipped by -SkipHap flag" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "================================================" -ForegroundColor Green
Write-Host "  Demo build pipeline complete!" -ForegroundColor Green
Write-Host "================================================" -ForegroundColor Green
