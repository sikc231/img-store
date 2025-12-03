# Quick Start Guide

## Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 10+)
- CMake 3.20+
- xxHash library
- Boost System & ASIO (for Crow)

## Installation

### 1. Install Dependencies

**Debian/Ubuntu/Kali:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libxxhash-dev libboost-system-dev libasio-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc-c++ cmake xxhash-devel boost-devel asio-devel
```

**macOS:**
```bash
brew install cmake xxhash boost asio
```

### 2. Build the Project

```bash
./build.sh
```

Or manually:
```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

### 3. Run the Server

```bash
./build/bin/img-store
```

**With custom options:**
```bash
./build/bin/img-store --port 9000 --storage /path/to/storage
```

## Using the API

### Upload an Image
```bash
curl -X POST http://localhost:8080/images --data-binary @photo.jpg
```

**Response:**
```json
{
  "id": "a1b2c3d4e5f6g7h8",
  "status": "uploaded",
  "size": 12345
}
```

### Download an Image
```bash
curl http://localhost:8080/images/a1b2c3d4e5f6g7h8 -o downloaded.jpg
```

### Delete an Image
```bash
curl -X DELETE http://localhost:8080/images/a1b2c3d4e5f6g7h8
```

### Health Check
```bash
curl http://localhost:8080/health
```

## Docker Deployment

### Build Docker Image
```bash
docker build -t img-store .
```

### Run with Docker
```bash
docker run -d -p 8080:8080 -v $(pwd)/storage:/app/storage img-store
```

### Using Docker Compose
```bash
docker-compose up -d
```

## Coolify Deployment

1. **Create New Project** in Coolify
2. **Connect GitHub Repository**: `sikc231/img-store`
3. **Set Build Configuration**:
   - Build Type: Dockerfile
   - Dockerfile path: `/Dockerfile`
4. **Configure Persistent Storage**:
   - Add volume mount: `/app/storage` → Persistent Volume
5. **Environment Variables** (optional):
   - `PORT=8080`
6. **Deploy** - Coolify will automatically rebuild on git push

### Persistent Storage Setup
To prevent image loss on rebuild:
- In Coolify dashboard → Storage → Add Persistent Volume
- Mount path: `/app/storage`
- This ensures images survive container restarts and rebuilds

## Testing

Run the test suite:
```bash
./test.sh
```

This will:
- Create a test image
- Upload it
- Download it
- Verify integrity
- Delete it
- Confirm deletion

## Project Structure

```
img-store/
├── CMakeLists.txt          # Build configuration
├── Dockerfile              # Container image
├── docker-compose.yml      # Local Docker setup
├── build.sh               # Build script
├── test.sh                # Test script
├── README.md              # Main documentation
├── API.md                 # API documentation
├── QUICKSTART.md          # This file
├── include/               # Header files
│   ├── server.h
│   ├── image_handler.h
│   ├── storage_manager.h
│   └── hash_utils.h
├── src/                   # Implementation
│   ├── main.cpp
│   ├── server.cpp
│   ├── image_handler.cpp
│   ├── storage_manager.cpp
│   └── hash_utils.cpp
└── storage/               # Image storage (runtime)
```

## Production Considerations

1. **Reverse Proxy**: Use nginx or Caddy for:
   - SSL/TLS termination
   - Rate limiting
   - CORS headers
   - Load balancing

2. **Backup**: Regularly backup the `storage/` directory

3. **Monitoring**: Add monitoring for:
   - Disk space
   - API response times
   - Error rates

4. **Security**:
   - Run behind firewall
   - Implement authentication if needed
   - Set up rate limiting

## Troubleshooting

### Build fails with "CMAKE_CXX_COMPILER not found"
```bash
sudo apt-get install build-essential g++
```

### "xxHash library not found"
```bash
sudo apt-get install libxxhash-dev
```

### Server starts but endpoints return 404
Check that you're using the correct paths:
- `/health` (not `/`)
- `/images` (not `/image`)

### Port already in use
```bash
./build/bin/img-store --port 9000
```

## Support

For issues and questions:
- Check [API.md](API.md) for endpoint documentation
- Review [README.md](README.md) for detailed information
- Check server logs for error messages
