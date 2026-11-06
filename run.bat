@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"
title Chrono-Cosmos Tree Simulation

echo ========================================================
echo    CHRONO-COSMOS // TREE SIMULATION LAUNCHER
echo ========================================================
echo.

:: Check if build\tree_server.exe exists, if not build it
if not exist "build\tree_server.exe" (
    echo [1/3] Server binary not found. Building with CMake and Ninja...
    if exist "C:\msys64\ucrt64\bin\cmake.exe" (
        "C:\msys64\ucrt64\bin\cmake.exe" -B build -G "Ninja" -DCMAKE_MAKE_PROGRAM="C:\msys64\ucrt64\bin\ninja.exe" -DCMAKE_CXX_COMPILER="C:\msys64\ucrt64\bin\g++.exe"
        "C:\msys64\ucrt64\bin\ninja.exe" -C build tree_server
    ) else (
        cmake -B build -G "Ninja"
        ninja -C build tree_server
    )

    if errorlevel 1 (
        echo.
        echo [ERROR] Build failed! Check the compiler errors above.
        pause
        exit /b 1
    )
)

echo [2/3] Launching C++ Backend Server on port 8080...
start /b "" "build\tree_server.exe" 8080

echo [3/3] Waiting for server and opening Interstellar GUI in browser...
timeout /t 1 /nobreak >nul
start http://127.0.0.1:8080

echo.
echo ========================================================
echo   Simulation running at: http://127.0.0.1:8080
echo   Press CTRL+C or close this window to exit.
echo ========================================================
echo.

:: Keep script alive until user exits
cmd /k

