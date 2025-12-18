#!/bin/bash

# Storage verification script for img-store
# This helps verify that persistent storage is working correctly

echo "======================================"
echo "   Storage Verification Test"
echo "======================================"
echo ""

# Configuration
SERVER_URL="${SERVER_URL:-http://localhost:8080}"
TEST_IMAGE_NAME="storage-test-$(date +%s)"

echo "Testing server at: $SERVER_URL"
echo "Test image name: $TEST_IMAGE_NAME"
echo ""

# Step 1: Check if server is running
echo "1. Checking if server is running..."
if curl -s -f "$SERVER_URL/health" > /dev/null 2>&1; then
    echo "   ✓ Server is running"
else
    echo "   ✗ Server is not responding"
    echo "   Please make sure the server is running"
    exit 1
fi
echo ""

# Step 2: Upload a test image
echo "2. Uploading test image..."
UPLOAD_RESPONSE=$(curl -s -X POST --data "Test data for storage verification at $(date)" \
    "$SERVER_URL/images/named/$TEST_IMAGE_NAME")

if echo "$UPLOAD_RESPONSE" | grep -q "uploaded"; then
    echo "   ✓ Upload successful"
    HASH=$(echo "$UPLOAD_RESPONSE" | grep -o '"hash":"[^"]*"' | cut -d'"' -f4)
    echo "   Image hash: $HASH"
else
    echo "   ✗ Upload failed"
    echo "   Response: $UPLOAD_RESPONSE"
    exit 1
fi
echo ""

# Step 3: Verify download works
echo "3. Verifying download..."
DOWNLOAD_RESPONSE=$(curl -s "$SERVER_URL/images/named/$TEST_IMAGE_NAME")

if echo "$DOWNLOAD_RESPONSE" | grep -q "Test data"; then
    echo "   ✓ Download successful"
else
    echo "   ✗ Download failed"
    echo "   Response: $DOWNLOAD_RESPONSE"
    exit 1
fi
echo ""

# Step 4: Check storage directory (if accessible)
echo "4. Checking storage directory..."
STORAGE_DIR="${STORAGE_DIR:-./storage}"

if [ -d "$STORAGE_DIR" ]; then
    echo "   Storage directory: $STORAGE_DIR"
    
    # Check if hash-based storage exists
    if find "$STORAGE_DIR" -type f -name "$HASH" 2>/dev/null | grep -q .; then
        echo "   ✓ Image file found in storage"
        FILE_PATH=$(find "$STORAGE_DIR" -type f -name "$HASH" 2>/dev/null | head -1)
        echo "   File path: $FILE_PATH"
    else
        echo "   ⚠ Image file not found in local storage"
        echo "   (This is normal if running in Docker without volume mounted to host)"
    fi
    
    # Check if name mapping exists
    if find "$STORAGE_DIR/names" -type f -name "$TEST_IMAGE_NAME.mapping" 2>/dev/null | grep -q .; then
        echo "   ✓ Name mapping found in storage"
        MAPPING_PATH=$(find "$STORAGE_DIR/names" -type f -name "$TEST_IMAGE_NAME.mapping" 2>/dev/null | head -1)
        echo "   Mapping path: $MAPPING_PATH"
        echo "   Mapping content: $(cat "$MAPPING_PATH")"
    else
        echo "   ⚠ Name mapping not found in local storage"
        echo "   (This is normal if running in Docker without volume mounted to host)"
    fi
else
    echo "   ⚠ Storage directory not accessible: $STORAGE_DIR"
    echo "   (This is normal if running in Docker)"
fi
echo ""

# Step 5: Instructions for restart test
echo "5. Restart persistence test"
echo "   To verify storage persists across restarts:"
echo ""
echo "   a) Restart your container/server now"
echo "   b) Wait for it to start"
echo "   c) Run this command:"
echo ""
echo "      curl $SERVER_URL/images/named/$TEST_IMAGE_NAME"
echo ""
echo "   If you see 'Test data for storage verification', storage is persistent! ✓"
echo "   If you see 'Image name not found', storage is NOT persistent! ✗"
echo ""

# Step 6: Cleanup option
echo "6. Cleanup"
echo "   To delete the test image, run:"
echo ""
echo "      curl -X DELETE $SERVER_URL/images/named/$TEST_IMAGE_NAME"
echo ""

echo "======================================"
echo "   Verification Complete"
echo "======================================"
echo ""
echo "Summary:"
echo "  • Test image uploaded: $TEST_IMAGE_NAME"
echo "  • Image hash: $HASH"
echo "  • Server URL: $SERVER_URL"
echo ""
echo "Next steps:"
echo "  1. Restart your container"
echo "  2. Run: curl $SERVER_URL/images/named/$TEST_IMAGE_NAME"
echo "  3. If it returns the test data, storage is working!"
echo ""
