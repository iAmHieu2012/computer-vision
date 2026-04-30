#!/bin/bash

# Define paths
EXEC="./bin/23122027"
INPUT="./images/input/test.jpg"
OUT_DIR="./images/output"

# 1. Check if the executable exists
if [ ! -f "$EXEC" ]; then
    echo "Error: Executable not found. Please run 'make' first!"
    exit 1
fi

# 2. Check if the input image exists
if [ ! -f "$INPUT" ]; then
    echo "Error: Input image not found at '$INPUT'."
    echo "Please create an 'images/input' directory and place a 'test.jpg' file inside it!"
    exit 1
fi

# 3. Create the output directory (if it doesn't exist)
mkdir -p "$OUT_DIR"

# Execute commands sequentially
echo "[1/2] Running Basic Harris Algorithm..."
$EXEC -harris $INPUT $OUT_DIR/harris_basic.jpg

echo "[2/2] Running Optimized Harris Algorithm..."
$EXEC -optHarris $INPUT $OUT_DIR/harris_opt.jpg

echo "Check the results in: $OUT_DIR "