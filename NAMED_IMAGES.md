# Named Image Storage

This guide explains how to use the named image storage feature in img-store.

## Overview

The img-store server now supports uploading and retrieving images using user-friendly names while maintaining the efficient hash-based sharded storage system underneath.

## Features

- **Upload by name**: Store images with custom names
- **Retrieve by name**: Download images using their names
- **Update names**: Re-upload to a name to update it with new content
- **Delete names**: Remove name mappings while preserving image data
- **Dual access**: Images remain accessible by both name and hash
- **Sharded storage**: Name mappings are stored in a sharded directory structure for efficiency

## API Endpoints

### Upload Image with Name
```bash
POST /images/named/<name>
```

Upload an image with a custom name. If the name already exists, it will be updated with the new content.

**Example:**
```bash
curl -X POST --data-binary @image.jpg http://localhost:8080/images/named/profile-photo
```

**Response (new upload):**
```json
{
  "status": "uploaded",
  "name": "profile-photo",
  "hash": "5a594bbcb30660a5",
  "size": 12345
}
```

**Response (update):**
```json
{
  "status": "updated",
  "name": "profile-photo",
  "hash": "ffedb7d312ca7d28",
  "size": 12345,
  "previous_hash": "5a594bbcb30660a5"
}
```

### Download Image by Name
```bash
GET /images/named/<name>
```

Download an image using its name.

**Example:**
```bash
curl http://localhost:8080/images/named/profile-photo > image.jpg
```

**Response Headers:**
- `Content-Type`: Detected MIME type (e.g., image/jpeg)
- `X-Image-Name`: The requested image name
- `X-Image-Hash`: The underlying hash ID

### Delete Name Mapping
```bash
DELETE /images/named/<name>
```

Remove a name mapping. Note: This only deletes the name-to-hash mapping; the actual image data is preserved and remains accessible by hash.

**Example:**
```bash
curl -X DELETE http://localhost:8080/images/named/profile-photo
```

**Response:**
```json
{
  "status": "deleted",
  "name": "profile-photo",
  "hash": "5a594bbcb30660a5",
  "note": "Name mapping removed. Image data preserved."
}
```

## Hash-Based Storage (Original)

The original hash-based endpoints remain fully functional:

### Upload Image (Hash-Based)
```bash
curl -X POST --data-binary @image.jpg http://localhost:8080/images
```

Returns:
```json
{
  "status": "uploaded",
  "id": "5a594bbcb30660a5",
  "size": 12345
}
```

### Download Image by Hash
```bash
curl http://localhost:8080/images/5a594bbcb30660a5 > image.jpg
```

### Delete Image by Hash
```bash
curl -X DELETE http://localhost:8080/images/5a594bbcb30660a5
```

## Storage Architecture

### Image Data Storage
Images are stored using content-based hashing with a 3-level sharded directory structure:

```
storage/
  24/38/fd/5a594bbcb30660a5  <- Actual image file
```

### Name Mapping Storage
Name mappings are stored separately with their own sharded structure based on name hash:

```
storage/
  names/
    6d/50/6b/profile-photo.mapping  <- Contains hash: "5a594bbcb30660a5"
```

This dual-layer approach provides:
- **Deduplication**: Multiple names can reference the same image data
- **Efficiency**: Both names and hashes use sharding for fast lookups
- **Flexibility**: Images accessible by both name and hash
- **Safety**: Deleting a name doesn't delete the image data

## Use Cases

### Content-Addressed Storage with Friendly Names
```bash
# Upload with name
curl -X POST --data-binary @logo.png http://localhost:8080/images/named/company-logo

# Multiple names can reference the same content
curl -X POST --data-binary @logo.png http://localhost:8080/images/named/brand-logo

# Both return the same image hash, but different names
```

### Version Management
```bash
# Upload initial version
curl -X POST --data-binary @logo-v1.png http://localhost:8080/images/named/current-logo

# Update to new version (old hash is preserved)
curl -X POST --data-binary @logo-v2.png http://localhost:8080/images/named/current-logo

# Old version still accessible by hash if needed
```

### Direct Access by Hash
```bash
# Upload by name
curl -X POST --data-binary @image.jpg http://localhost:8080/images/named/photo
# Returns: {"name":"photo","hash":"abc123...", ...}

# Access directly by hash (faster, no name lookup)
curl http://localhost:8080/images/abc123...
```

## Authentication

If API key authentication is enabled (via `IMG_STORE_API_KEY` environment variable):

- **GET requests**: Public (no authentication required)
- **POST/DELETE requests**: Protected (API key required)

Include the API key in your requests:

```bash
# Using Authorization header
curl -H "Authorization: Bearer YOUR_API_KEY" -X POST --data-binary @image.jpg \
  http://localhost:8080/images/named/photo

# Using X-API-Key header
curl -H "X-API-Key: YOUR_API_KEY" -X POST --data-binary @image.jpg \
  http://localhost:8080/images/named/photo
```

## Benefits

1. **User-Friendly**: Use meaningful names instead of hashes
2. **Efficient**: Sharded storage for both names and data
3. **Deduplication**: Same content uploaded multiple times uses same storage
4. **Flexible**: Access by name or hash
5. **Safe**: Deleting names doesn't lose data
6. **Fast**: Hash-based lookups with minimal overhead
