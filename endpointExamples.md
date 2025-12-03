# Image Storage Server - API Endpoint Examples

Complete reference for all API endpoints with practical examples.

---

## Base URL

```
http://localhost:8080
```

For production, replace with your domain (e.g., `https://images.yourdomain.com`)

---

## Authentication

### API Key Location

Protected endpoints require an API key in the request header:

**Option 1: Authorization Bearer Token**
```
Authorization: Bearer YOUR_API_KEY
```

**Option 2: X-API-Key Header**
```
X-API-Key: YOUR_API_KEY
```

### Getting Your API Key

```bash
# From .env file
cat .env | grep IMG_STORE_API_KEY

# Or extract just the value
grep IMG_STORE_API_KEY .env | cut -d'=' -f2
```

---

## Endpoints

### 1. Health Check

**Public** - No authentication required

Check if the server is running and healthy.

**Request:**
```http
GET /health HTTP/1.1
Host: localhost:8080
```

**cURL:**
```bash
curl http://localhost:8080/health
```

**Response:**
```json
{
  "service": "img-store",
  "status": "healthy"
}
```

**Status Codes:**
- `200 OK` - Server is healthy

---

### 2. Upload Image

**Protected** - API key required

Upload an image or binary file. Images are stored by their content hash (automatic deduplication).

**Request:**
```http
POST /images HTTP/1.1
Host: localhost:8080
Authorization: Bearer YOUR_API_KEY
Content-Type: application/octet-stream

[binary image data]
```

**cURL Examples:**

```bash
# Upload with Bearer token
curl -X POST http://localhost:8080/images \
  -H "Authorization: Bearer ac93fd799187970deedfea28f666b7855e7002e45fdd7db221c7e49339f2a8a0" \
  --data-binary @photo.jpg

# Upload with X-API-Key header
curl -X POST http://localhost:8080/images \
  -H "X-API-Key: ac93fd799187970deedfea28f666b7855e7002e45fdd7db221c7e49339f2a8a0" \
  --data-binary @photo.jpg

# Upload using environment variable
API_KEY=$(grep IMG_STORE_API_KEY .env | cut -d'=' -f2)
curl -X POST http://localhost:8080/images \
  -H "Authorization: Bearer $API_KEY" \
  --data-binary @image.png
```

**Python:**
```python
import requests

api_key = "ac93fd799187970deedfea28f666b7855e7002e45fdd7db221c7e49339f2a8a0"

with open('photo.jpg', 'rb') as f:
    response = requests.post(
        'http://localhost:8080/images',
        data=f,
        headers={'Authorization': f'Bearer {api_key}'}
    )
    print(response.json())
```

**JavaScript (Node.js):**
```javascript
const fs = require('fs');
const fetch = require('node-fetch');

const apiKey = 'ac93fd799187970deedfea28f666b7855e7002e45fdd7db221c7e49339f2a8a0';
const imageData = fs.readFileSync('photo.jpg');

fetch('http://localhost:8080/images', {
    method: 'POST',
    body: imageData,
    headers: {
        'Authorization': `Bearer ${apiKey}`
    }
})
.then(res => res.json())
.then(json => console.log(json));
```

**Response (Success - New Image):**
```json
{
  "id": "a1b2c3d4e5f6g7h8",
  "status": "uploaded",
  "size": 245678
}
```

**Response (Success - Existing Image):**
```json
{
  "id": "a1b2c3d4e5f6g7h8",
  "status": "exists"
}
```

**Response (Error - No Auth):**
```json
{
  "error": "Unauthorized",
  "message": "API key required for write operations"
}
```

**Status Codes:**
- `201 Created` - Image uploaded successfully
- `200 OK` - Image already exists (deduplication)
- `400 Bad Request` - Empty or invalid image data
- `401 Unauthorized` - Missing or invalid API key
- `500 Internal Server Error` - Server error

---

### 3. Download Image

**Public** - No authentication required

Retrieve an image by its ID. Content-Type is automatically detected.

**Request:**
```http
GET /images/{imageId} HTTP/1.1
Host: localhost:8080
```

**cURL Examples:**

```bash
# Download to file
curl http://localhost:8080/images/a1b2c3d4e5f6g7h8 -o downloaded.jpg

# Download and view headers
curl -I http://localhost:8080/images/a1b2c3d4e5f6g7h8

# Download with progress bar
curl -# http://localhost:8080/images/a1b2c3d4e5f6g7h8 -o image.jpg
```

**wget:**
```bash
wget http://localhost:8080/images/a1b2c3d4e5f6g7h8 -O image.jpg
```

**Python:**
```python
import requests

image_id = 'a1b2c3d4e5f6g7h8'
response = requests.get(f'http://localhost:8080/images/{image_id}')

if response.status_code == 200:
    with open('downloaded.jpg', 'wb') as f:
        f.write(response.content)
    print(f"Content-Type: {response.headers['Content-Type']}")
```

**JavaScript (Browser):**
```javascript
const imageId = 'a1b2c3d4e5f6g7h8';

fetch(`http://localhost:8080/images/${imageId}`)
    .then(res => res.blob())
    .then(blob => {
        const url = URL.createObjectURL(blob);
        const img = document.createElement('img');
        img.src = url;
        document.body.appendChild(img);
    });
```

**HTML (Direct Link):**
```html
<img src="http://localhost:8080/images/a1b2c3d4e5f6g7h8" alt="Image">
```

**Response:**
- Binary image data with appropriate `Content-Type` header

**Status Codes:**
- `200 OK` - Image found and returned
- `404 Not Found` - Image doesn't exist
- `500 Internal Server Error` - Server error

**Content-Type Headers:**
- `image/jpeg` - JPEG images
- `image/png` - PNG images
- `image/gif` - GIF images
- `image/bmp` - BMP images
- `image/webp` - WebP images
- `application/octet-stream` - Unknown format

---

### 4. Delete Image

**Protected** - API key required

Delete an image by its ID. This permanently removes the file from storage.

**Request:**
```http
DELETE /images/{imageId} HTTP/1.1
Host: localhost:8080
Authorization: Bearer YOUR_API_KEY
```

**cURL Examples:**

```bash
# Delete with Bearer token
curl -X DELETE http://localhost:8080/images/a1b2c3d4e5f6g7h8 \
  -H "Authorization: Bearer ac93fd799187970deedfea28f666b7855e7002e45fdd7db221c7e49339f2a8a0"

# Delete with X-API-Key header
curl -X DELETE http://localhost:8080/images/a1b2c3d4e5f6g7h8 \
  -H "X-API-Key: ac93fd799187970deedfea28f666b7855e7002e45fdd7db221c7e49339f2a8a0"

# Delete using environment variable
API_KEY=$(grep IMG_STORE_API_KEY .env | cut -d'=' -f2)
curl -X DELETE http://localhost:8080/images/a1b2c3d4e5f6g7h8 \
  -H "Authorization: Bearer $API_KEY"
```

**Python:**
```python
import requests

api_key = "ac93fd799187970deedfea28f666b7855e7002e45fdd7db221c7e49339f2a8a0"
image_id = "a1b2c3d4e5f6g7h8"

response = requests.delete(
    f'http://localhost:8080/images/{image_id}',
    headers={'Authorization': f'Bearer {api_key}'}
)
print(response.json())
```

**JavaScript (Node.js):**
```javascript
const fetch = require('node-fetch');

const apiKey = 'ac93fd799187970deedfea28f666b7855e7002e45fdd7db221c7e49339f2a8a0';
const imageId = 'a1b2c3d4e5f6g7h8';

fetch(`http://localhost:8080/images/${imageId}`, {
    method: 'DELETE',
    headers: {
        'Authorization': `Bearer ${apiKey}`
    }
})
.then(res => res.json())
.then(json => console.log(json));
```

**Response (Success):**
```json
{
  "id": "a1b2c3d4e5f6g7h8",
  "status": "deleted"
}
```

**Response (Error - No Auth):**
```json
{
  "error": "Unauthorized",
  "message": "API key required for write operations"
}
```

**Status Codes:**
- `200 OK` - Image deleted successfully
- `401 Unauthorized` - Missing or invalid API key
- `404 Not Found` - Image doesn't exist
- `500 Internal Server Error` - Failed to delete

---

## Complete Workflow Example

### Bash Script

```bash
#!/bin/bash

# Get API key from .env
API_KEY=$(grep IMG_STORE_API_KEY .env | cut -d'=' -f2)
BASE_URL="http://localhost:8080"

# 1. Check health
echo "1. Health check..."
curl -s $BASE_URL/health | jq .
echo ""

# 2. Upload image
echo "2. Uploading image..."
UPLOAD_RESPONSE=$(curl -s -X POST $BASE_URL/images \
  -H "Authorization: Bearer $API_KEY" \
  --data-binary @photo.jpg)
echo "$UPLOAD_RESPONSE" | jq .

# Extract image ID
IMAGE_ID=$(echo "$UPLOAD_RESPONSE" | jq -r '.id')
echo "Image ID: $IMAGE_ID"
echo ""

# 3. Download image
echo "3. Downloading image..."
curl -s $BASE_URL/images/$IMAGE_ID -o downloaded.jpg
echo "Downloaded to: downloaded.jpg"
echo ""

# 4. Verify file
echo "4. Verifying integrity..."
if cmp -s photo.jpg downloaded.jpg; then
    echo "✓ Files match!"
else
    echo "✗ Files don't match"
fi
echo ""

# 5. Delete image
echo "5. Deleting image..."
curl -s -X DELETE $BASE_URL/images/$IMAGE_ID \
  -H "Authorization: Bearer $API_KEY" | jq .
echo ""

# 6. Verify deletion
echo "6. Verifying deletion..."
HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" $BASE_URL/images/$IMAGE_ID)
if [ "$HTTP_CODE" = "404" ]; then
    echo "✓ Image deleted (404)"
else
    echo "✗ Image still exists (HTTP $HTTP_CODE)"
fi

# Cleanup
rm -f downloaded.jpg
```

### Python Script

```python
import requests
import os

# Configuration
API_KEY = os.getenv('IMG_STORE_API_KEY', 'your-api-key')
BASE_URL = 'http://localhost:8080'
headers = {'Authorization': f'Bearer {API_KEY}'}

# 1. Health check
print("1. Health check...")
r = requests.get(f'{BASE_URL}/health')
print(r.json())

# 2. Upload image
print("\n2. Uploading image...")
with open('photo.jpg', 'rb') as f:
    r = requests.post(f'{BASE_URL}/images', data=f, headers=headers)
    result = r.json()
    print(result)
    image_id = result['id']

# 3. Download image
print(f"\n3. Downloading image {image_id}...")
r = requests.get(f'{BASE_URL}/images/{image_id}')
with open('downloaded.jpg', 'wb') as f:
    f.write(r.content)
print(f"Content-Type: {r.headers['Content-Type']}")

# 4. Delete image
print(f"\n4. Deleting image...")
r = requests.delete(f'{BASE_URL}/images/{image_id}', headers=headers)
print(r.json())

# 5. Verify deletion
r = requests.get(f'{BASE_URL}/images/{image_id}')
print(f"\n5. Verification: HTTP {r.status_code} ({'Deleted' if r.status_code == 404 else 'Still exists'})")

# Cleanup
os.remove('downloaded.jpg')
```

---

## Testing with Postman

### 1. Health Check
- Method: `GET`
- URL: `http://localhost:8080/health`
- Headers: None
- Body: None

### 2. Upload Image
- Method: `POST`
- URL: `http://localhost:8080/images`
- Headers:
  - `Authorization: Bearer YOUR_API_KEY`
- Body:
  - Type: `binary`
  - Select file: Choose your image

### 3. Download Image
- Method: `GET`
- URL: `http://localhost:8080/images/{imageId}`
- Headers: None
- Body: None
- Response: Save to file

### 4. Delete Image
- Method: `DELETE`
- URL: `http://localhost:8080/images/{imageId}`
- Headers:
  - `Authorization: Bearer YOUR_API_KEY`
- Body: None

---

## Error Handling

### Common Errors

**401 Unauthorized**
```json
{
  "error": "Unauthorized",
  "message": "API key required for write operations"
}
```
**Solution:** Add `Authorization` or `X-API-Key` header with valid API key

**404 Not Found**
```
Image not found
```
**Solution:** Check image ID is correct and image exists

**400 Bad Request**
```
Empty image data
```
**Solution:** Ensure file is not empty and data is binary

**500 Internal Server Error**
```
Internal server error
```
**Solution:** Check server logs, ensure storage directory is writable

---

## Notes

- Image IDs are 16-character hexadecimal strings (e.g., `a1b2c3d4e5f6g7h8`)
- Same image content = same ID (automatic deduplication)
- Images stored in sharded structure: `storage/ab/cd/ef/a1b2c3d4e5f6g7h8`
- Maximum file size: Limited by available disk space
- Supported formats: Any binary data (JPEG, PNG, GIF, BMP, WebP auto-detected)
