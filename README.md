# Image Storage Server - Made with a big help of Claude Sonnet 4.5

A high-performance C++20 HTTP server for storing, retrieving, and managing images with a sharded filesystem layout and API key authentication.

## ⚠️ Important: Coolify/Docker Deployment

**If images disappear after restart**, you need to configure persistent storage!

👉 **Quick Fix:** See [COOLIFY_STORAGE_FIX.md](COOLIFY_STORAGE_FIX.md) for step-by-step instructions.

In Coolify, add a volume mapping:
- **Source (Host):** `/var/lib/coolify/storage/img-store`
- **Destination (Container):** `/app/storage`

## Features

- **HTTP REST API** using Crow web framework
- **Named image storage** - Upload and retrieve images using friendly names
- **API Key Authentication** - Public downloads, protected uploads/deletes
- **Hash-based sharding** using xxHash3 for efficient filesystem organization
- **Dual access modes**: Access images by name or by content hash
- **Image operations**: Upload, Download, Delete (by name or hash)
- **Content-type detection** for common image formats (JPEG, PNG, GIF, BMP, WebP)
- **Content-addressable storage** - images are stored by their content hash (automatic deduplication)
- **Docker & Coolify ready** - Easy deployment with persistent storage support

## Requirements

- C++20 compatible compiler (GCC 10+, Clang 10+)
- CMake 3.20 or higher
- [Crow](https://github.com/CrowCpp/Crow) HTTP framework (header-only)
- [xxHash](https://github.com/Cyan4973/xxHash) library

## Building

### Install Dependencies

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libxxhash-dev libboost-system-dev libasio-dev
```

#### Fedora/RHEL
```bash
sudo dnf install gcc-c++ cmake xxhash-devel boost-devel asio-devel
```

#### macOS
```bash
brew install cmake xxhash boost asio
```

### Get Crow (if not system-installed)

```bash
cd lib/
wget https://raw.githubusercontent.com/CrowCpp/Crow/master/include/crow_all.h
# Or use git submodule if preferred
### Build the Project

**Using build script (recommended):**
```bash
./build.sh
```

**Or manually:**
```bash
mkdir build
cd build
cmake ..
make
```

The binary will be located at `build/bin/img-store`.

### Generate API Key

```bash
./setup-auth.sh
```

This creates a `.env` file with a secure random API key.

The binary will be located at `build/bin/img-store`.

## Usage

### Start the Server

**With authentication (recommended):**
```bash
# Load API key from .env file
API_KEY=$(grep IMG_STORE_API_KEY .env | cut -d'=' -f2)
IMG_STORE_API_KEY=$API_KEY ./build/bin/img-store
```

**Without authentication (testing only):**
```bash
./build/bin/img-store
# ⚠️ WARNING: All endpoints will be public!
```

### Command-line Options

```bash
./build/bin/img-store [OPTIONS]

Options:
  -p, --port <port>        Server port (default: 8080)
  -s, --storage <dir>      Storage directory (default: ./storage)
  -k, --api-key <key>      API key for write operations
  -h, --help               Show help message

Environment Variables:
  IMG_STORE_API_KEY        API key (alternative to --api-key)
```

### Example

```bash
# Start server on port 9000 with custom storage and API key
./build/bin/img-store --port 9000 --storage /var/img-store --api-key "your-secret-key"
```

## API Endpoints

See [NAMED_IMAGES.md](NAMED_IMAGES.md) for detailed documentation on named image storage.

### Quick Reference

#### Named Image Endpoints (Friendly Names)
- `POST /images/named/<name>` - Upload image with custom name (Protected)
- `GET /images/named/<name>` - Download image by name (Public)
- `DELETE /images/named/<name>` - Delete name mapping (Protected)

#### Hash-Based Endpoints (Content-Addressed)
- `POST /images` - Upload image, returns hash (Protected)
- `GET /images/<hash>` - Download image by hash (Public)
- `DELETE /images/<hash>` - Delete image by hash (Protected)
- `GET /health` - Health check (Public)

### Authentication

- **Public (no auth required):** `GET /health`, `GET /images/{id}`, `GET /images/named/{name}`
- **Protected (API key required):** `POST /images`, `POST /images/named/{name}`, `DELETE /images/{id}`, `DELETE /images/named/{name}`

API key must be provided via:
- `Authorization: Bearer YOUR_API_KEY` header, or
- `X-API-Key: YOUR_API_KEY` header

### Health Check (Public)
```bash
GET /health
```

**Response:**
```json
{
  "status": "healthy",
  "service": "img-store"
}
```

### Upload Image by Name (Protected)
```bash
POST /images/named/<name>
Authorization: Bearer YOUR_API_KEY
Content-Type: application/octet-stream
Body: <binary image data>
```

**Response:**
```json
{
  "name": "profile-photo",
  "hash": "a1b2c3d4e5f6g7h8",
  "status": "uploaded",
  "size": 12345
}
```

### Download Image by Name (Public)
```bash
GET /images/named/<name>
```

**Response:** Binary image data with appropriate `Content-Type` header and `X-Image-Hash` header

### Upload Image by Hash (Protected)
```bash
POST /images
Authorization: Bearer YOUR_API_KEY
Content-Type: application/octet-stream
Body: <binary image data>
```

**Response:**
```json
{
  "id": "a1b2c3d4e5f6g7h8",
  "status": "uploaded",
  "size": 12345
}
```

**Without API key:**
```json
{
  "error": "Unauthorized",
  "message": "API key required for write operations"
}
```

### Download Image by Hash (Public)
```bash
GET /images/<image-id>
```

**Response:** Binary image data with appropriate `Content-Type` header

### Delete Image (Protected)
```bash
DELETE /images/<image-id>
Authorization: Bearer YOUR_API_KEY
```

**Response:**
```json
{
  "id": "a1b2c3d4e5f6g7h8",
  "status": "deleted"
}
```

## Examples

### Named Image Storage (Recommended)
```bash
# Get API key from .env
API_KEY=$(grep IMG_STORE_API_KEY .env | cut -d'=' -f2)

# Upload with a friendly name
curl -X POST http://localhost:8080/images/named/company-logo \
  -H "Authorization: Bearer $API_KEY" \
  --data-binary @logo.png

# Response: {"name":"company-logo","hash":"a1b2c3...","status":"uploaded","size":12345}

# Download by name (no auth needed)
curl http://localhost:8080/images/named/company-logo > downloaded-logo.png

# Update the image (upload new content to same name)
curl -X POST http://localhost:8080/images/named/company-logo \
  -H "Authorization: Bearer $API_KEY" \
  --data-binary @new-logo.png
```

### Hash-Based Storage (Direct)
```bash
# Get API key from .env
API_KEY=$(grep IMG_STORE_API_KEY .env | cut -d'=' -f2)

# Upload with Bearer token
curl -X POST http://localhost:8080/images \
  -H "Authorization: Bearer $API_KEY" \
  --data-binary @photo.jpg

# Or with X-API-Key header
curl -X POST http://localhost:8080/images \
  -H "X-API-Key: $API_KEY" \
  --data-binary @photo.jpg
```

### Download an image (public, no auth)
```bash
curl http://localhost:8080/images/a1b2c3d4e5f6g7h8 \
  -o downloaded.jpg
```

### Delete an image (requires API key)
```bash
API_KEY=$(grep IMG_STORE_API_KEY .env | cut -d'=' -f2)

curl -X DELETE http://localhost:8080/images/a1b2c3d4e5f6g7h8 \
  -H "Authorization: Bearer $API_KEY"
```

**📖 For complete examples in multiple languages, see [endpointExamples.md](endpointExamples.md)**

## Architecture

### Sharded Storage Layout

Images are stored in a sharded directory structure based on their content hash:

```
storage/
├── ab/
│   ├── cd/
│   │   ├── ef/
│   │   │   └── a1b2c3d4e5f6g7h8
```

This approach:
- Prevents directory overload (too many files in one directory)
- Provides efficient lookup and storage
- Uses content-addressable storage (deduplication)

### Components

- **Server** (`server.h/cpp`): HTTP server setup and routing with auth
- **ImageHandler** (`image_handler.h/cpp`): Request handling logic
- **StorageManager** (`storage_manager.h/cpp`): Filesystem operations with sharding
- **HashUtils** (`hash_utils.h/cpp`): xxHash3 wrapper and shard path generation
- **AuthMiddleware** (`auth_middleware.h/cpp`): API key authentication

## Docker Deployment

### Using Docker Compose (Recommended)

```bash
# Generate API key first
./setup-auth.sh

# Build and run
docker-compose up -d

# View logs
docker-compose logs -f

# Stop
docker-compose down
```

The `.env` file is automatically loaded, and images are persisted in `./storage` directory.

### Manual Docker Build

```bash
# Build image
docker build -t img-store .

# Run with API key
docker run -d \
  -p 8080:8080 \
  -v $(pwd)/storage:/app/storage \
  -e IMG_STORE_API_KEY="your-api-key-here" \
  img-store
```

## Coolify Deployment

**⚠️ Important:** Configure persistent storage or images will be lost on restart!

See [COOLIFY_STORAGE_FIX.md](COOLIFY_STORAGE_FIX.md) for detailed setup instructions.

### Quick Deploy Steps

1. **Push code to GitHub**

2. **In Coolify:**
   - Create new application from GitHub repository
   - Dockerfile is auto-detected

3. **⚠️ Configure Persistent Storage (REQUIRED):**
   - Go to **"Storages"** or **"Volumes"** tab
   - Add volume:
     - **Source (Host):** `/var/lib/coolify/storage/img-store`
     - **Destination (Container):** `/app/storage`
   - Without this, images will disappear on restart!

4. **Add Environment Variable (Optional):**
   - Name: `IMG_STORE_API_KEY`
   - Value: Your API key (from `.env` or generate new one)
   - **Mark as Secret** ✓ (to hide from logs)

5. **Deploy**

### Verify Storage Persistence

After deployment, use the verification script:
```bash
# Set your deployed URL
SERVER_URL="https://your-app.coolify.app" ./verify-storage.sh
```

Then restart the container in Coolify and verify images persist.

See [DEPLOYMENT.md](DEPLOYMENT.md) for complete deployment guide.

Images will persist across updates and rebuilds when using persistent volumes.

## Security

- **Public access:** Anyone can download images (GET requests)
- **Protected operations:** Upload and delete require API key (POST/DELETE)
- **API key storage:** Use environment variables, never commit `.env` to git
- **Production:** Use HTTPS with reverse proxy (nginx, Caddy, Traefik)
- **Best practices:** See [SECURITY.md](SECURITY.md) and [AUTH_GUIDE.md](AUTH_GUIDE.md)

## Documentation

- **[COOLIFY_STORAGE_FIX.md](COOLIFY_STORAGE_FIX.md)** - Fix for images disappearing after restart ⚠️
- **[NAMED_IMAGES.md](NAMED_IMAGES.md)** - Named image storage feature guide
- **[DEPLOYMENT.md](DEPLOYMENT.md)** - Coolify deployment guide
- **[endpointExamples.md](endpointExamples.md)** - Complete API examples in cURL, Python, JavaScript
- **[AUTH_GUIDE.md](AUTH_GUIDE.md)** - Authentication setup and usage
- **[SECURITY.md](SECURITY.md)** - Security configuration and best practices
- **[API.md](API.md)** - Detailed API reference

## Scripts

- **`verify-storage.sh`** - Test if persistent storage is working correctly
- **`named-image-examples.sh`** - Examples for named image storage
- **`verify.sh`** - Comprehensive API testing
- **`build.sh`** - Build the project
- **`setup-auth.sh`** - Generate API key

## Testing

Run comprehensive tests:
```bash
./verify.sh
```

This tests all endpoints including authentication.

## Development

### Project Structure
```
img-store/
├── CMakeLists.txt           # Build configuration
├── Dockerfile               # Container image
├── docker-compose.yml       # Docker setup
├── build.sh                 # Build script
├── setup-auth.sh            # API key generator
├── verify.sh                # Test script
├── README.md                # This file
├── endpointExamples.md      # Complete API examples
├── AUTH_GUIDE.md            # Authentication guide
├── SECURITY.md              # Security documentation
├── .env.example             # Environment template
├── .gitignore
├── include/                 # Header files
│   ├── server.h
│   ├── image_handler.h
│   ├── storage_manager.h
│   ├── hash_utils.h
│   ├── auth_middleware.h
│   └── crow_all.h
├── src/                     # Implementation files
│   ├── main.cpp
│   ├── server.cpp
│   ├── image_handler.cpp
│   ├── storage_manager.cpp
│   ├── hash_utils.cpp
│   └── auth_middleware.cpp
├── lib/                     # Third-party libraries
├── storage/                 # Image storage directory
└── tests/                   # Test files
``` tests/               # Test files (future)
```
