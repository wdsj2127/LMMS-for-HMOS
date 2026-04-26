param(
    [string]$LmmsBuildDir = "..\build_lmms_ohos",
    [string]$QtInstallDir = "C:\Qt\qt-5.15.16-ohos",
    [string]$HapProjectDir = ".",
    [string]$TargetArch = "arm64-v8a"
)

$ErrorActionPreference = "Stop"

$libsDir = Join-Path $HapProjectDir "entry\libs\$TargetArch"
if (-not (Test-Path $libsDir)) {
    New-Item -ItemType Directory -Force -Path $libsDir | Out-Null
    Write-Host "Created directory: $libsDir"
}

Write-Host "=== Deploying LMMS for HarmonyOS ==="
Write-Host "LMMS build dir: $LmmsBuildDir"
Write-Host "Qt install dir:  $QtInstallDir"
Write-Host "Target arch:      $TargetArch"
Write-Host ""

# 1. Copy LMMS main library
Write-Host "--- Copying LMMS library ---"
$lmmsLib = Join-Path $LmmsBuildDir "liblmms.so"
if (Test-Path $lmmsLib) {
    Copy-Item $lmmsLib $libsDir -Force
    Write-Host "  Copied: liblmms.so"
} else {
    Write-Warning "  NOT FOUND: $lmmsLib"
}

# 2. Copy Qt runtime libraries
Write-Host "--- Copying Qt runtime libraries ---"
$qtLibs = @("libQt5Core.so", "libQt5Gui.so", "libQt5Widgets.so", "libQt5Xml.so", "libQt5Svg.so")
foreach ($lib in $qtLibs) {
    $src = Join-Path $QtInstallDir "lib\$lib"
    if (Test-Path $src) {
        Copy-Item $src $libsDir -Force
        Write-Host "  Copied: $lib"
    } else {
        Write-Warning "  NOT FOUND: $src"
    }
}

# 3. Copy Qt platform plugin
Write-Host "--- Copying Qt platform plugin ---"
$platformPlugin = Join-Path $QtInstallDir "plugins\platforms\libqtaf_ohos.so"
if (Test-Path $platformPlugin) {
    Copy-Item $platformPlugin $libsDir -Force
    Write-Host "  Copied: libqtaf_ohos.so"
} else {
    Write-Warning "  NOT FOUND: $platformPlugin"
}

# 4. Copy third-party dependency libraries
Write-Host "--- Copying third-party dependencies ---"
$depLibs = @(
    "libsndfile.so", "libFLAC.so", "libogg.so", "libvorbis.so", "libvorbisenc.so",
    "libfftw3f.so", "libsamplerate.so", "libfluidsynth.so", "libglib-2.0.so"
)
foreach ($lib in $depLibs) {
    $src = Join-Path $LmmsBuildDir "ohos-deps\lib\$lib"
    if (Test-Path $src) {
        Copy-Item $src $libsDir -Force
        Write-Host "  Copied: $lib"
    } else {
        $src2 = Join-Path $LmmsBuildDir "lib\$lib"
        if (Test-Path $src2) {
            Copy-Item $src2 $libsDir -Force
            Write-Host "  Copied: $lib"
        }
    }
}

# 5. Copy LMMS plugins
Write-Host "--- Copying LMMS plugins ---"
$pluginsDir = Join-Path $HapProjectDir "entry\libs\$TargetArch\plugins"
if (-not (Test-Path $pluginsDir)) {
    New-Item -ItemType Directory -Force -Path $pluginsDir | Out-Null
}

$lmmsPluginsDir = Join-Path $LmmsBuildDir "plugins"
if (Test-Path $lmmsPluginsDir) {
    $pluginLibs = Get-ChildItem -Path $lmmsPluginsDir -Filter "*.so" -Recurse
    foreach ($plugin in $pluginLibs) {
        Copy-Item $plugin.FullName $pluginsDir -Force
        Write-Host "  Copied plugin: $($plugin.Name)"
    }
}

# 6. Summary
Write-Host ""
Write-Host "=== Deployment Summary ==="
$totalFiles = (Get-ChildItem -Path $libsDir -File).Count
$totalSize = (Get-ChildItem -Path $libsDir -File | Measure-Object -Property Length -Sum).Sum / 1MB
Write-Host "  Files deployed: $totalFiles"
Write-Host "  Total size:     $([math]::Round($totalSize, 2)) MB"
Write-Host "  Target dir:     $libsDir"
Write-Host ""
Write-Host "Next steps:"
Write-Host "  1. Open DevEco Studio and load the HAP project"
Write-Host "  2. Configure signing: File -> Project Structure -> Signing Configs"
Write-Host "  3. Build and run on HarmonyOS device"
