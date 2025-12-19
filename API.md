# API Documentation

Base URL: `http://your-domain.com`

## Authentication

- **GET** requests are public (no authentication required)
- **POST/DELETE** requests require API key

Set API key via header:
```bash
Authorization: Bearer YOUR_API_KEY
# or
X-API-Key: YOUR_API_KEY
```

---

## Named Image Endpoints

### Upload Named Image
```http
POST /{name}
```

Upload an image with a custom name. The name becomes the URL path.

**Headers:**
- `Authorization: Bearer YOUR_API_KEY` (required)
- `Content-Type: image/*` (optional, auto-detected)

**Body:** Binary image data

**Example:**
```bash
curl -X POST http://your-domain.com/logo.png \
  -H "Authorization: Bearer your-key" \
  --data-binary @image.png
```

**Response (201):**
```json
{
  "success": true,
  "imageId": "a1b2c3d4e5f6...",
  "name": "logo.png",
  "url": "/logo.png",
  "hashUrl": "/images/a1b2c3d4e5f6..."
}
```

---

### Download Named Image
```http
GET /{name}
```

Retrieve an image by its name. Public access, no authentication needed.

**Example:**
```bash
curl http://your-domain.com/logo.png -o downloaded.png
```

**Response (200):**
- Binary image data
- `Content-Type` header set to appropriate image MIME type

**Response (404):**
```json
{
  "error": "Image not found"
}
```

---

### Delete Named Image
```http
DELETE /{name}
```

Delete the name mapping for an image (underlying image data persists if referenced elsewhere).

**Headers:**
- `Authorization: Bearer YOUR_API_KEY` (required)

**Example:**
```bash
curl -X DELETE http://your-domain.com/logo.png \
  -H "Authorization: Bearer your-key"
```

**Response (200):**
```json
{
  "success": true,
  "message": "Name mapping deleted"
}
```

---

## Hash-Based Endpoints

### Upload Image (Hash)
```http
POST /images
```

Upload an image and get its content hash. Images are deduplicated by content.

**Headers:**
- `Authorization: Bearer YOUR_API_KEY` (required)
- `Content-Type: image/*` (optional)

**Body:** Binary image data

**Example:**
```bash
curl -X POST http://your-domain.com/images \
  -H "Authorization: Bearer your-key" \
  --data-binary @image.png
```

**Response (201):**
```json
{
  "success": true,
  "imageId": "a1b2c3d4e5f67890",
  "url": "/images/a1b2c3d4e5f67890"
}
```

---

### Download Image (Hash)
```http
GET /images/{hash}
```

Retrieve an image by its content hash. Public access.

**Example:**
```bash
curl http://your-domain.com/images/a1b2c3d4e5f67890 -o image.png
```

**Response (200):**
- Binary image data
- `Content-Type` header set appropriately

**Response (404):**
```json
{
  "error": "Image not found"
}
```

---

### Delete Image (Hash)
```http
DELETE /images/{hash}
```

Delete an image by its hash.

**Headers:**
- `Authorization: Bearer YOUR_API_KEY` (required)

**Example:**
```bash
curl -X DELETE http://your-domain.com/images/a1b2c3d4e5f67890 \
  -H "Authorization: Bearer your-key"
```

**Response (200):**
```json
{
  "success": true,
  "message": "Image deleted"
}
```

---

## Health Check

### Health Status
```http
GET /health
```

Check if the server is running. Public endpoint.

**Example:**
```bash
curl http://your-domain.com/health
```

**Response (200):**
```json
{
  "status": "healthy",
  "service": "img-store"
}
```

---

## List Images

### Get All Image Names
```http
GET /images/names
```

Retrieve a list of all named images. Public endpoint.

**Example:**
```bash
curl http://your-domain.com/images/names
```

**Response (200):**
```json
{
  "count": 3,
  "names": [
    "logo.png",
    "profile-pic.jpg",
    "banner.webp"
  ]
}
```

---

## Error Responses

### 401 Unauthorized
```json
{
  "error": "Unauthorized",
  "message": "API key required for write operations"
}
```

### 404 Not Found
```json
{
  "error": "Image not found"
}
```

### 500 Internal Server Error
```json
{
  "error": "Failed to store image"
}
```

---

## Content Types

Supported image formats (auto-detected):
- `image/jpeg` (.jpg, .jpeg)
- `image/png` (.png)
- `image/gif` (.gif)
- `image/webp` (.webp)
- `image/bmp` (.bmp)

---

## Usage Examples

### Complete Workflow
```bash
# 1. Upload with custom name
curl -X POST http://your-domain.com/profile-pic.jpg \
  -H "Authorization: Bearer your-key" \
  --data-binary @photo.jpg

# 2. Access the image (public)
curl http://your-domain.com/profile-pic.jpg -o downloaded.jpg

# 3. Upload without name (get hash)
curl -X POST http://your-domain.com/images \
  -H "Authorization: Bearer your-key" \
  --data-binary @document.png

# Response: {"imageId": "abc123...", ...}

# 4. Access by hash (public)
curl http://your-domain.com/images/abc123... -o doc.png

# 5. Delete named image
curl -X DELETE http://your-domain.com/profile-pic.jpg \
  -H "Authorization: Bearer your-key"
```

### HTML Integration
```html
<!-- Direct image embed -->
<img src="http://your-domain.com/logo.png" alt="Logo">

<!-- Using hash -->
<img src="http://your-domain.com/images/a1b2c3d4e5f67890" alt="Image">
```

### JavaScript Fetch
```javascript
// Upload
const file = document.getElementById('fileInput').files[0];
const response = await fetch('http://your-domain.com/my-image.png', {
  method: 'POST',
  headers: {
    'Authorization': 'Bearer your-key'
  },
  body: file
});
const result = await response.json();

// Download
const img = document.createElement('img');
img.src = 'http://your-domain.com/my-image.png';
```
