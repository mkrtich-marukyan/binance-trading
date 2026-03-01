#!/bin/bash
# ===============================
# Build script for Binance Trading
# ===============================

set -e  # Exit immediately if any command fails

# Default options
CLEAN_BUILD=true
RUN_TESTS=true
RUN_APP=true   # Run if everything succeeds

# Parse command line arguments
for arg in "$@"; do
    case $arg in
        --no-clean)
            CLEAN_BUILD=false
            shift
            ;;
        --skip-tests)
            RUN_TESTS=false
            shift
            ;;
        --no-run)
            RUN_APP=false
            shift
            ;;
        *)
            echo "[WARN] Unknown argument: $arg"
            ;;
    esac
done

# Activate Python virtualenv for Conan
VENV_PATH="$HOME/.conan2/bin/activate"

if [ -f "$VENV_PATH" ]; then
    echo "[INFO] Activating Conan Python virtualenv..."
    source "$VENV_PATH"
else
    echo "[WARN] Virtualenv not found at $VENV_PATH, make sure Conan is installed!"
fi

# Clean previous build (optional)
if [ "$CLEAN_BUILD" = true ]; then
    echo "[INFO] Cleaning previous build folder..."
    rm -rf build
else
    echo "[INFO] Skipping clean build"
fi

# Install dependencies with Conan
echo "[INFO] Installing Conan dependencies..."
conan install . -b missing

# Configure CMake using Conan preset
echo "[INFO] Configuring CMake..."
cmake --preset conan-release

# Build the project
echo "[INFO] Building project..."
cmake --build --preset conan-release

# Run unit tests (optional)
if [ "$RUN_TESTS" = true ]; then
    echo "[INFO] Running unit tests..."
    ctest --preset conan-release --output-on-failure
else
    echo "[INFO] Skipping unit tests"
fi

echo "[SUCCESS] Build and tests successful!"

# Run Binance Trading (optional)
if [ "$RUN_APP" = true ]; then
    echo "[INFO] Running Binance Trading..."
    ./build/Release/binance-trading
else
    echo "[INFO] Skipping execution"
fi
