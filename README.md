# Image Storage Server

A high-performance C++20 HTTP server for storing, retrieving, and managing images with a sharded filesystem layout.

## Features

- **HTTP REST API** using Crow web framework
- **Hash-based sharding** using xxHash3 for efficient filesystem organization
- **Image operations**: Upload, Download, Delete
- **Content-type detection** for common image formats (JPEG, PNG, GIF, BMP, WebP)
- **Content-addressable storage** - images are stored by their content hash

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
```

### Build the Project

```bash
mkdir build
cd build
cmake ..
make
```

The binary will be located at `build/bin/img-store`.

## Usage

### Start the Server

```bash
./build/bin/img-store
```

### Command-line Options

```bash
./build/bin/img-store [OPTIONS]

Options:
  -p, --port <port>        Server port (default: 8080)
  -s, --storage <dir>      Storage directory (default: ./storage)
  -h, --help               Show help message
```

### Example

```bash
# Start server on port 9000 with custom storage directory
./build/bin/img-store --port 9000 --storage /var/img-store
```

## API Endpoints

### Health Check
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

### Upload Image
```bash
POST /images
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

### Download Image
```bash
GET /images/<image-id>
```

**Response:** Binary image data with appropriate `Content-Type` header

### Delete Image
```bash
DELETE /images/<image-id>
```

**Response:**
```json
{
  "id": "a1b2c3d4e5f6g7h8",
  "status": "deleted"
}
```

## Examples

### Upload an image
```bash
curl -X POST http://localhost:8080/images \
  --data-binary @photo.jpg
```

### Download an image
```bash
curl http://localhost:8080/images/a1b2c3d4e5f6g7h8 \
  -o downloaded.jpg
```

### Delete an image
```bash
curl -X DELETE http://localhost:8080/images/a1b2c3d4e5f6g7h8
```

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

- **Server** (`server.h/cpp`): HTTP server setup and routing
- **ImageHandler** (`image_handler.h/cpp`): Request handling logic
- **StorageManager** (`storage_manager.h/cpp`): Filesystem operations with sharding
- **HashUtils** (`hash_utils.h/cpp`): xxHash3 wrapper and shard path generation

## Development

### Project Structure

```
img-store/
├── CMakeLists.txt       # Build configuration
├── README.md
├── .gitignore
├── include/             # Header files
│   ├── server.h
│   ├── image_handler.h
│   ├── storage_manager.h
│   └── hash_utils.h
├── src/                 # Implementation files
│   ├── main.cpp
│   ├── server.cpp
│   ├── image_handler.cpp
│   ├── storage_manager.cpp
│   └── hash_utils.cpp
├── lib/                 # Third-party libraries (header-only)
├── storage/             # Default image storage directory
└── tests/               # Test files (future)
```

## License

MIT License - see LICENSE file for details
