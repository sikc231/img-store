#!/bin/bash

# Build script for img-store project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}   Image Storage Server - Build Script${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# Check dependencies
echo -e "${YELLOW}Checking dependencies...${NC}"

if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: cmake not found. Please install cmake.${NC}"
    exit 1
fi

if ! command -v g++ &> /dev/null; then
    echo -e "${RED}Error: g++ not found. Please install build-essential.${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Dependencies OK${NC}"
echo ""

# Create build directory
echo -e "${YELLOW}Creating build directory...${NC}"
mkdir -p build
cd build

# Configure with CMake
echo -e "${YELLOW}Configuring with CMake...${NC}"
cmake .. || {
    echo -e "${RED}CMake configuration failed!${NC}"
    exit 1
}
echo -e "${GREEN}✓ Configuration complete${NC}"
echo ""

# Build
echo -e "${YELLOW}Building project...${NC}"
make -j$(nproc) || {
    echo -e "${RED}Build failed!${NC}"
    exit 1
}
echo -e "${GREEN}✓ Build complete${NC}"
echo ""

# Check if binary exists
if [ -f "bin/img-store" ]; then
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}   Build Successful!${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo ""
    echo -e "Executable: ${GREEN}build/bin/img-store${NC}"
    echo ""
    echo -e "To run the server:"
    echo -e "  ${YELLOW}./build/bin/img-store${NC}"
    echo ""
    echo -e "Or with custom options:"
    echo -e "  ${YELLOW}./build/bin/img-store --port 9000 --storage /path/to/storage${NC}"
    echo ""
else
    echo -e "${RED}Error: Binary not found after build!${NC}"
    exit 1
fi
