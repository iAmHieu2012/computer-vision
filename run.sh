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

echo "========================================"
echo "      STARTING IMAGE PROCESSING         "
echo "========================================"

# Execute commands sequentially
echo "[1/10] Converting to Grayscale..."
$EXEC -rgb2gray $INPUT $OUT_DIR/1_gray.jpg

echo "[2/10] Adjusting Brightness (+50)..."
$EXEC -brightness $INPUT $OUT_DIR/2_brightness_50.jpg 50

echo "[3/10] Adjusting Contrast (1.5x)..."
$EXEC -contrast $INPUT $OUT_DIR/3_contrast_1.5.jpg 1.5

echo "[4/10] Applying Average Filter (k=5)..."
$EXEC -avg $INPUT $OUT_DIR/4_avg_k5.jpg 5

echo "[5/10] Applying Median Filter (k=5)..."
$EXEC -med $INPUT $OUT_DIR/5_med_k5.jpg 5

echo "[6/10] Applying Gaussian Filter (k=5)..."
$EXEC -gau $INPUT $OUT_DIR/6_gau_k5.jpg 5

echo "[7/10] Applying Sobel Edge Detection..."
$EXEC -sobel $INPUT $OUT_DIR/7_sobel.jpg

echo "[8/10] Applying Laplace Edge Detection..."
$EXEC -laplace $INPUT $OUT_DIR/8_laplace.jpg

echo "[9/10] Applying Basic Harris Corner Detection..."
$EXEC -harris $INPUT $OUT_DIR/9_harris_basic.jpg

echo "[10/10] Applying Optimized Harris Corner Detection..."
$EXEC -optHarris $INPUT $OUT_DIR/10_harris_optimized.jpg

echo "========================================"
echo "ALL DONE! Check the results in: $OUT_DIR"