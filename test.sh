#!/bin/bash

# Quick test script for the image storage server

set -e

SERVER_URL="http://localhost:8080"
TEST_IMAGE="test_image.jpg"

echo "==================================="
echo "   Image Storage Server - Tests"
echo "==================================="
echo ""

# Create a test image (random data)
echo "Creating test image..."
dd if=/dev/urandom of=$TEST_IMAGE bs=1024 count=10 2>/dev/null
echo "✓ Test image created (10KB)"
echo ""

# Test 1: Health check
echo "Test 1: Health Check"
if curl -s "$SERVER_URL/health" | grep -q "healthy"; then
    echo "✓ Health check passed"
else
    echo "✗ Health check failed"
    exit 1
fi
echo ""

# Test 2: Upload image
echo "Test 2: Upload Image"
RESPONSE=$(curl -s -X POST "$SERVER_URL/images" --data-binary @$TEST_IMAGE)
IMAGE_ID=$(echo $RESPONSE | grep -o '"id":"[^"]*"' | cut -d'"' -f4)

if [ -n "$IMAGE_ID" ]; then
    echo "✓ Upload successful"
    echo "  Image ID: $IMAGE_ID"
else
    echo "✗ Upload failed"
    echo "  Response: $RESPONSE"
    exit 1
fi
echo ""

# Test 3: Download image
echo "Test 3: Download Image"
if curl -s -o downloaded_$TEST_IMAGE "$SERVER_URL/images/$IMAGE_ID"; then
    echo "✓ Download successful"
else
    echo "✗ Download failed"
    exit 1
fi
echo ""

# Test 4: Verify image integrity
echo "Test 4: Verify Image Integrity"
if cmp -s $TEST_IMAGE downloaded_$TEST_IMAGE; then
    echo "✓ Image integrity verified"
else
    echo "✗ Image integrity check failed"
    exit 1
fi
echo ""

# Test 5: Delete image
echo "Test 5: Delete Image"
DELETE_RESPONSE=$(curl -s -X DELETE "$SERVER_URL/images/$IMAGE_ID")
if echo $DELETE_RESPONSE | grep -q "deleted"; then
    echo "✓ Delete successful"
else
    echo "✗ Delete failed"
    echo "  Response: $DELETE_RESPONSE"
    exit 1
fi
echo ""

# Test 6: Verify deletion
echo "Test 6: Verify Deletion"
HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL/images/$IMAGE_ID")
if [ "$HTTP_CODE" = "404" ]; then
    echo "✓ Image properly deleted (404)"
else
    echo "✗ Image still exists (HTTP $HTTP_CODE)"
    exit 1
fi
echo ""

# Cleanup
rm -f $TEST_IMAGE downloaded_$TEST_IMAGE

echo "==================================="
echo "   All Tests Passed! ✓"
echo "==================================="
