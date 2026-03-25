#!/bin/bash
set -e

VERSION="1.1.0"
BUILD_DIR="build"
INSTALL_DIR="simpledbms-${VERSION}"

# Clean previous builds
rm -rf "$BUILD_DIR" "$INSTALL_DIR"

# Build
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Install to staging directory
make DESTDIR="../$INSTALL_DIR" install
echo "✓ Build complete: $INSTALL_DIR/"