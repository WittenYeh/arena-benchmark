#!/bin/bash

# Build script for arena-benchmark

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Arena Benchmark Build Script ===${NC}"

# Default build type
BUILD_TYPE=${1:-Release}
BUILD_DIR="build"

echo -e "${YELLOW}Build type: ${BUILD_TYPE}${NC}"

# Create build directory
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning existing build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo -e "${GREEN}Configuring CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DBUILD_EXAMPLES=ON

# Build
echo -e "${GREEN}Building...${NC}"
cmake --build . -j$(nproc)

echo -e "${GREEN}=== Build completed successfully! ===${NC}"
echo -e "${YELLOW}Executables are in: ${BUILD_DIR}/examples/${NC}"
echo -e "${YELLOW}Run example: ./build/examples/basic_example${NC}"
