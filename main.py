"""
Chrono-Cosmos // Tree Simulation Entry Point
Run via: python main.py
"""

import os
import sys
import subprocess
import time
import webbrowser
from pathlib import Path

ROOT = Path(__file__).parent.resolve()
BUILD_DIR = ROOT / "build"
SERVER_EXE = BUILD_DIR / "tree_server.exe"

def build_if_needed():
    if not SERVER_EXE.exists():
        print("[1/3] Building C++ tree_server with CMake and Ninja...")
        cmake_bin = r"C:\msys64\ucrt64\bin\cmake.exe"
        ninja_bin = r"C:\msys64\ucrt64\bin\ninja.exe"
        gcc_bin = r"C:\msys64\ucrt64\bin\g++.exe"

        if Path(cmake_bin).exists():
            subprocess.run([cmake_bin, "-B", str(BUILD_DIR), "-G", "Ninja", 
                           f"-DCMAKE_MAKE_PROGRAM={ninja_bin}", f"-DCMAKE_CXX_COMPILER={gcc_bin}"], check=True)
            subprocess.run([ninja_bin, "-C", str(BUILD_DIR), "tree_server"], check=True)
        else:
            subprocess.run(["cmake", "-B", str(BUILD_DIR), "-G", "Ninja"], check=True)
            subprocess.run(["ninja", "-C", str(BUILD_DIR), "tree_server"], check=True)

def main():
    print("=" * 60)
    print("   CHRONO-COSMOS // TREE SIMULATION LAUNCHER (Python)")
    print("=" * 60)
    
    build_if_needed()

    print("[2/3] Launching C++ Backend Server on http://127.0.0.1:8080...")
    proc = subprocess.Popen([str(SERVER_EXE), "8080"], cwd=str(ROOT))

    time.sleep(1.0)
    print("[3/3] Opening Interstellar Frontend in your default browser...")
    webbrowser.open("http://127.0.0.1:8080")

    print("\n" + "=" * 60)
    print("  Simulation active! Press Ctrl+C in this terminal to exit.")
    print("=" * 60 + "\n")

    try:
        proc.wait()
    except KeyboardInterrupt:
        print("\n[INFO] Stopping backend server...")
        proc.terminate()
        proc.wait()
        print("[INFO] Server stopped.")

if __name__ == "__main__":
    main()
