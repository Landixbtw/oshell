#!/bin/bash

# Default build type
BUILD_TYPE="Debug"
BUILD_DIR="build"
SANITIZER=""

# Help message
show_help() {
    echo "Usage: ./build.sh [options]"
    echo "Options:"
    echo "  -h, --help           Show this help message"
    echo "  -c, --clean          Clean build directory before building"
    echo "  -r, --release        Build in Release mode"
    echo "  -a, --asan          Build with Address Sanitizer"
    echo "  -m, --msan          Build with Memory Sanitizer"
    echo "  -t, --tsan          Build with Thread Sanitizer"
    echo "  -u, --ubsan         Build with Undefined Behavior Sanitizer"
}

# Process command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -c|--clean)
            CLEAN=1
            shift
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -a|--asan)
            SANITIZER="ASAN"
            shift
            ;;
        -m|--msan)
            SANITIZER="MSAN"
            shift
            ;;
        -t|--tsan)
            SANITIZER="TSAN"
            shift
            ;;
        -u|--ubsan)
            SANITIZER="UBSAN"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Clean build directory if requested
if [[ -n "$CLEAN" ]]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Base CMake command with English language
CMAKE_CMD="LANG=C cmake"

# Add sanitizer flag if specified
if [[ -n "$SANITIZER" ]]; then
    CMAKE_CMD="$CMAKE_CMD -DENABLE_${SANITIZER}=ON"
fi

# Add build type
CMAKE_CMD="$CMAKE_CMD -DCMAKE_BUILD_TYPE=$BUILD_TYPE .."

# Run CMake
echo "Running: $CMAKE_CMD"
eval "$CMAKE_CMD"

# Build the project
echo "Building..."
cmake --build .

# Check build status
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Binary location: $BUILD_DIR/oshell"
else
    echo "Build failed!"
    exit 1
fi
