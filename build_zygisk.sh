#!/bin/bash

# Pastikan ANDROID_NDK_HOME sudah diset di environment Anda
if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "ERROR: ANDROID_NDK_HOME environment variable is not set."
    echo "Please set it to your Android NDK path (e.g., export ANDROID_NDK_HOME=/path/to/android-ndk-r26b)"
    # return instead of exit to avoid closing the interactive shell session if sourced,
    # but since this is a script, we'll just skip the rest of the script.
else
    CMAKE_TOOLCHAIN="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake"

    if [ ! -f "$CMAKE_TOOLCHAIN" ]; then
        echo "ERROR: CMake toolchain not found at $CMAKE_TOOLCHAIN"
    else
        ABIS=("armeabi-v7a" "arm64-v8a" "x86" "x86_64")

        echo "Building Zygisk Mock Props module..."

        for ABI in "${ABIS[@]}"; do
            echo "=========================================="
            echo "Building for ABI: $ABI"
            echo "=========================================="

            BUILD_DIR="build/$ABI"
            mkdir -p "$BUILD_DIR"

            cmake -Hzygisk -B"$BUILD_DIR" \
                -DANDROID_ABI="$ABI" \
                -DANDROID_PLATFORM=android-27 \
                -DCMAKE_TOOLCHAIN_FILE="$CMAKE_TOOLCHAIN" \
                -DANDROID_NDK="$ANDROID_NDK_HOME" \
                -DCMAKE_BUILD_TYPE=Release

            cmake --build "$BUILD_DIR" --target mockprops

            # Zygisk mengharapkan file .so disimpan di root folder zip module sebagai zygisk/<abi>.so
            if [ -f "$BUILD_DIR/libmockprops.so" ]; then
                cp "$BUILD_DIR/libmockprops.so" "zygisk/${ABI}.so"
                echo "Successfully copied libmockprops.so to zygisk/${ABI}.so"
            else
                echo "ERROR: Build failed for $ABI"
            fi
        done

        echo "Build complete! Check the zygisk/ directory for the compiled .so files."
    fi
fi
