#!/bin/bash

# Default values
BUILD_TYPE="debug"
BUILD_DIR="build"
SANITIZER=""
CLEAN=0

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
    echo ""
    echo "Note: MSAN only works with Clang"
    echo "      TSAN can't be combined with ASAN"
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
            BUILD_TYPE="release"
            shift
            ;;
        -a|--asan)
            SANITIZER="address"
            shift
            ;;
        -m|--msan)
            SANITIZER="memory"
            shift
            ;;
        -t|--tsan)
            SANITIZER="thread"
            shift
            ;;
        -u|--ubsan)
            SANITIZER="undefined"
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
if [[ $CLEAN -eq 1 ]]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Meson setup command
MESON_OPTS="-Dbuildtype=$BUILD_TYPE"

# Add sanitizer if specified
if [[ -n "$SANITIZER" ]]; then
    MESON_OPTS="$MESON_OPTS -Db_sanitize=$SANITIZER"
fi

# Setup build directory
echo "Setting up build with: meson setup $BUILD_DIR $MESON_OPTS"
LANG=C meson setup "$BUILD_DIR" $MESON_OPTS

# Check setup status
if [ $? -ne 0 ]; then
    echo "Meson setup failed!"
    exit 1
fi

# Build the project
echo "Building..."
ninja -C "$BUILD_DIR"

# Check build status
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Binary location: $BUILD_DIR/oshell"
else
    echo "Build failed!"
    exit 1
fi
