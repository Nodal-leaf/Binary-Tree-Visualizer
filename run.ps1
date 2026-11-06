# Chrono-Cosmos Universal Entry-Point Script (PowerShell)
# Usage: .\run.ps1

$ErrorActionPreference = "Stop"
Set-Location -Path $PSScriptRoot

Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "   CHRONO-COSMOS // TREE SIMULATION LAUNCHER (PowerShell)" -ForegroundColor Cyan
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host ""

$serverExe = Join-Path $PSScriptRoot "build\tree_server.exe"

# 1. Build server if missing
if (-not (Test-Path $serverExe)) {
    Write-Host "[1/3] Server binary not found. Building with CMake and Ninja..." -ForegroundColor Yellow
    $cmakePath = "C:\msys64\ucrt64\bin\cmake.exe"
    $ninjaPath = "C:\msys64\ucrt64\bin\ninja.exe"
    $gccPath = "C:\msys64\ucrt64\bin\g++.exe"

    if (Test-Path $cmakePath) {
        & $cmakePath -B build -G "Ninja" -DCMAKE_MAKE_PROGRAM="$ninjaPath" -DCMAKE_CXX_COMPILER="$gccPath"
        & $ninjaPath -C build tree_server
    } else {
        cmake -B build -G "Ninja"
        ninja -C build tree_server
    }

    if (-not (Test-Path $serverExe)) {
        Write-Host "[ERROR] Failed to compile tree_server.exe!" -ForegroundColor Red
        exit 1
    }
}

# 2. Start C++ backend process
Write-Host "[2/3] Launching C++ Backend Server on http://127.0.0.1:8080..." -ForegroundColor Green
$serverProcess = Start-Process -FilePath $serverExe -ArgumentList "8080" -PassThru -NoNewWindow

# 3. Open browser
Start-Sleep -Seconds 1
Write-Host "[3/3] Opening Interstellar Frontend in your default browser..." -ForegroundColor Green
Start-Process "http://127.0.0.1:8080"

Write-Host ""
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "  Simulation is LIVE at: http://127.0.0.1:8080" -ForegroundColor Cyan
Write-Host "  Press Ctrl+C to stop the server." -ForegroundColor Yellow
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host ""

try {
    # Keep script running and forward server logs
    $serverProcess.WaitForExit()
} finally {
    if (-not $serverProcess.HasExited) {
        Write-Host "`n[INFO] Stopping backend server..." -ForegroundColor Yellow
        Stop-Process -Id $serverProcess.Id -Force -ErrorAction SilentlyContinue
    }
}
