# API Documentation

## Base URL
```
http://localhost:8080
```

## Endpoints

### 1. Health Check
Check if the server is running.

**Endpoint:** `GET /health`

**Response:**
```json
{
  "status": "healthy",
  "service": "img-store"
}
```

**Example:**
```bash
curl http://localhost:8080/health
```

---

### 2. Upload Image
Upload a new image to the server.

**Endpoint:** `POST /images`

**Headers:**
- `Content-Type: application/octet-stream` (or specific image type)

**Body:** Binary image data

**Response (201 Created):**
```json
{
  "id": "a1b2c3d4e5f6g7h8",
  "status": "uploaded",
  "size": 12345
}
```

**Response (200 OK - if image already exists):**
```json
{
  "id": "a1b2c3d4e5f6g7h8",
  "status": "exists"
}
```

**Examples:**

Using curl:
```bash
# Upload from file
curl -X POST http://localhost:8080/images \
  --data-binary @image.jpg

# Upload with specific content type
curl -X POST http://localhost:8080/images \
  -H "Content-Type: image/jpeg" \
  --data-binary @photo.jpg
```

Using Python:
```python
import requests

with open('image.jpg', 'rb') as f:
    response = requests.post('http://localhost:8080/images', data=f)
    print(response.json())
```

Using JavaScript (Node.js):
```javascript
const fs = require('fs');
const fetch = require('node-fetch');

const imageData = fs.readFileSync('image.jpg');
fetch('http://localhost:8080/images', {
    method: 'POST',
    body: imageData,
    headers: { 'Content-Type': 'image/jpeg' }
})
.then(res => res.json())
.then(json => console.log(json));
```

---

### 3. Download Image
Retrieve an image by its ID.

**Endpoint:** `GET /images/{imageId}`

**Path Parameters:**
- `imageId` (string): The unique identifier of the image

**Response:** Binary image data with appropriate `Content-Type` header

**Status Codes:**
- `200 OK`: Image found and returned
- `404 Not Found`: Image doesn't exist

**Examples:**

Using curl:
```bash
# Download to file
curl http://localhost:8080/images/a1b2c3d4e5f6g7h8 \
  -o downloaded_image.jpg

# View headers
curl -I http://localhost:8080/images/a1b2c3d4e5f6g7h8
```

Using wget:
```bash
wget http://localhost:8080/images/a1b2c3d4e5f6g7h8 \
  -O image.jpg
```

Using Python:
```python
import requests

response = requests.get('http://localhost:8080/images/a1b2c3d4e5f6g7h8')
if response.status_code == 200:
    with open('downloaded.jpg', 'wb') as f:
        f.write(response.content)
```

Using JavaScript (Browser):
```javascript
fetch('http://localhost:8080/images/a1b2c3d4e5f6g7h8')
    .then(res => res.blob())
    .then(blob => {
        const url = URL.createObjectURL(blob);
        const img = document.createElement('img');
        img.src = url;
        document.body.appendChild(img);
    });
```

---

### 4. Delete Image
Delete an image by its ID.

**Endpoint:** `DELETE /images/{imageId}`

**Path Parameters:**
- `imageId` (string): The unique identifier of the image

**Response (200 OK):**
```json
{
  "id": "a1b2c3d4e5f6g7h8",
  "status": "deleted"
}
```

**Status Codes:**
- `200 OK`: Image successfully deleted
- `404 Not Found`: Image doesn't exist
- `500 Internal Server Error`: Failed to delete

**Examples:**

Using curl:
```bash
curl -X DELETE http://localhost:8080/images/a1b2c3d4e5f6g7h8
```

Using Python:
```python
import requests

response = requests.delete('http://localhost:8080/images/a1b2c3d4e5f6g7h8')
print(response.json())
```

Using JavaScript:
```javascript
fetch('http://localhost:8080/images/a1b2c3d4e5f6g7h8', {
    method: 'DELETE'
})
.then(res => res.json())
.then(json => console.log(json));
```

---

## Image ID Generation

Images are identified by their **content hash** using xxHash3:
- Same image content = same ID (automatic deduplication)
- Different content = different ID
- ID format: 16-character hexadecimal string

## Content Type Detection

The server automatically detects common image formats:
- **JPEG**: `image/jpeg`
- **PNG**: `image/png`
- **GIF**: `image/gif`
- **BMP**: `image/bmp`
- **WebP**: `image/webp`
- **Unknown**: `application/octet-stream`

## Error Responses

All errors return appropriate HTTP status codes:

**400 Bad Request:**
```json
"Empty image data"
```

**404 Not Found:**
```json
"Image not found"
```

**500 Internal Server Error:**
```json
"Internal server error"
```

## Storage Layout

Images are stored in a sharded filesystem structure:
```
storage/
├── ab/
│   ├── cd/
│   │   ├── ef/
│   │   │   └── a1b2c3d4e5f6g7h8
```

This prevents directory overload and enables efficient lookups.

## Rate Limiting

Currently, no rate limiting is implemented. Consider adding a reverse proxy (nginx, Caddy) for production use.

## CORS

CORS headers are not set by default. To enable cross-origin requests, add a reverse proxy configuration.
