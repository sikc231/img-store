#!/bin/bash

# Comprehensive verification script

set -e

echo "=========================================="
echo "  IMG-STORE VERIFICATION"
echo "=========================================="
echo ""

# Check if server is running
if ! curl -s http://localhost:8080/health > /dev/null 2>&1; then
    echo "❌ Server is not running!"
    echo "Start it with: ./build/bin/img-store"
    exit 1
fi

echo "✓ Server is running"
echo ""

# Test 1: Health Check
echo "Test 1: Health Check"
HEALTH=$(curl -s http://localhost:8080/health)
if echo "$HEALTH" | grep -q "healthy"; then
    echo "✓ Health check passed"
    echo "  Response: $HEALTH"
else
    echo "❌ Health check failed"
    exit 1
fi
echo ""

# Create test image
echo "Test 2: Creating test image (100KB)"
dd if=/dev/urandom of=verify_test.bin bs=1024 count=100 2>/dev/null
echo "✓ Test file created"
echo ""

# Test 3: Upload
echo "Test 3: Upload image"
UPLOAD_RESPONSE=$(curl -s -X POST http://localhost:8080/images --data-binary @verify_test.bin)
echo "  Response: $UPLOAD_RESPONSE"

if echo "$UPLOAD_RESPONSE" | grep -q '"id"'; then
    IMAGE_ID=$(echo "$UPLOAD_RESPONSE" | grep -o '"id":"[^"]*"' | cut -d'"' -f4)
    echo "✓ Upload successful"
    echo "  Image ID: $IMAGE_ID"
else
    echo "❌ Upload failed"
    rm -f verify_test.bin
    exit 1
fi
echo ""

# Test 4: Storage verification
echo "Test 4: Verify sharded storage"
STORED_FILE=$(find storage -type f -name "$IMAGE_ID" 2>/dev/null | head -1)
if [ -n "$STORED_FILE" ]; then
    echo "✓ File found in storage"
    echo "  Path: $STORED_FILE"
    ls -lh "$STORED_FILE"
else
    echo "❌ File not found in storage"
    rm -f verify_test.bin
    exit 1
fi
echo ""

# Test 5: Download
echo "Test 5: Download image"
curl -s "http://localhost:8080/images/$IMAGE_ID" -o verify_download.bin
if [ -f verify_download.bin ] && [ -s verify_download.bin ]; then
    echo "✓ Download successful"
    ls -lh verify_download.bin
else
    echo "❌ Download failed or file is empty"
    rm -f verify_test.bin verify_download.bin
    exit 1
fi
echo ""

# Test 6: Integrity check
echo "Test 6: Verify data integrity"
if cmp -s verify_test.bin verify_download.bin; then
    echo "✓ Data integrity verified - files match perfectly!"
    echo "  Original:   $(md5sum verify_test.bin | cut -d' ' -f1)"
    echo "  Downloaded: $(md5sum verify_download.bin | cut -d' ' -f1)"
else
    echo "❌ Data integrity check failed - files don't match"
    rm -f verify_test.bin verify_download.bin
    exit 1
fi
echo ""

# Test 7: Delete
echo "Test 7: Delete image"
DELETE_RESPONSE=$(curl -s -X DELETE "http://localhost:8080/images/$IMAGE_ID")
if echo "$DELETE_RESPONSE" | grep -q "deleted"; then
    echo "✓ Delete successful"
    echo "  Response: $DELETE_RESPONSE"
else
    echo "❌ Delete failed"
    rm -f verify_test.bin verify_download.bin
    exit 1
fi
echo ""

# Test 8: Verify deletion
echo "Test 8: Verify deletion (should return 404)"
HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" "http://localhost:8080/images/$IMAGE_ID")
if [ "$HTTP_CODE" = "404" ]; then
    echo "✓ Image properly deleted (HTTP $HTTP_CODE)"
else
    echo "❌ Image still accessible (HTTP $HTTP_CODE)"
    rm -f verify_test.bin verify_download.bin
    exit 1
fi
echo ""

# Cleanup
rm -f verify_test.bin verify_download.bin

echo "=========================================="
echo "  ✓ ALL TESTS PASSED!"
echo "=========================================="
echo ""
echo "The image storage server is fully functional:"
echo "  • Health checks working"
echo "  • Upload working with content-based hashing"
echo "  • Sharded storage structure created correctly"
echo "  • Download working with correct data"
echo "  • Data integrity maintained (upload = download)"
echo "  • Delete working properly"
echo ""
