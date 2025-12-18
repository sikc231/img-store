#!/bin/bash
# Examples for using named image storage

SERVER="http://localhost:8080"

echo "======================================"
echo "   Named Image Storage Examples"
echo "======================================"
echo ""

# Example 1: Upload with name
echo "1. Upload an image with a name:"
echo "   curl -X POST --data 'Sample image content' $SERVER/images/named/logo"
curl -X POST --data "Sample image content" $SERVER/images/named/logo
echo -e "\n"

# Example 2: Retrieve by name
echo "2. Download the image by name:"
echo "   curl $SERVER/images/named/logo"
curl $SERVER/images/named/logo
echo -e "\n\n"

# Example 3: Update existing name
echo "3. Update the image (upload new content to same name):"
echo "   curl -X POST --data 'Updated image content' $SERVER/images/named/logo"
curl -X POST --data "Updated image content" $SERVER/images/named/logo
echo -e "\n"

# Example 4: Verify update
echo "4. Verify the update:"
echo "   curl $SERVER/images/named/logo"
curl $SERVER/images/named/logo
echo -e "\n\n"

# Example 5: Upload another named image
echo "5. Upload another named image:"
echo "   curl -X POST --data 'Avatar image data' $SERVER/images/named/user-avatar"
curl -X POST --data "Avatar image data" $SERVER/images/named/user-avatar
echo -e "\n"

# Example 6: List what we have
echo "6. Both images are now accessible:"
echo "   curl $SERVER/images/named/logo"
curl $SERVER/images/named/logo
echo -e "\n"
echo "   curl $SERVER/images/named/user-avatar"
curl $SERVER/images/named/user-avatar
echo -e "\n\n"

# Example 7: Access by hash (get hash from upload response)
echo "7. Images are also accessible by their hash ID"
echo "   The hash is returned in the upload response and in X-Image-Hash header"
echo ""

# Example 8: Delete name mapping
echo "8. Delete a name mapping:"
echo "   curl -X DELETE $SERVER/images/named/user-avatar"
curl -X DELETE $SERVER/images/named/user-avatar
echo -e "\n"

# Example 9: Verify deletion
echo "9. Name is no longer accessible:"
echo "   curl $SERVER/images/named/user-avatar"
curl $SERVER/images/named/user-avatar
echo -e "\n\n"

echo "======================================"
echo "   Examples Complete!"
echo "======================================"
echo ""
echo "Note: The actual image data is preserved even after"
echo "deleting the name mapping, and can still be accessed"
echo "using the hash ID directly via /images/<hash>"
