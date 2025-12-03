# Authentication Summary

## ✅ Implementation Complete

Your image storage server now has API key authentication enabled!

## How It Works

- **GET requests** - Always public (no auth required)
- **POST/DELETE requests** - Require API key

## Your API Key

Check your `.env` file:
```bash
cat .env
```

## Local Usage

### Start Server
```bash
# Load API key from .env and start
cd /home/kali/img-store
API_KEY=$(grep IMG_STORE_API_KEY .env | cut -d'=' -f2)
IMG_STORE_API_KEY=$API_KEY ./build/bin/img-store
```

### Public Endpoints (No Auth)
```bash
# Health check
curl http://localhost:8080/health

# Download image
curl http://localhost:8080/images/{id} -o image.jpg
```

### Protected Endpoints (Require API Key)
```bash
# Get your API key
API_KEY=$(grep IMG_STORE_API_KEY .env | cut -d'=' -f2)

# Upload with Bearer token
curl -X POST http://localhost:8080/images \
  -H "Authorization: Bearer $API_KEY" \
  --data-binary @photo.jpg

# Upload with X-API-Key header
curl -X POST http://localhost:8080/images \
  -H "X-API-Key: $API_KEY" \
  --data-binary @photo.jpg

# Delete image
curl -X DELETE http://localhost:8080/images/{id} \
  -H "Authorization: Bearer $API_KEY"
```

## Docker Deployment

### Using docker-compose
```bash
# .env file is automatically loaded
docker-compose up -d
```

### Manual Docker run
```bash
docker run -d \
  -p 8080:8080 \
  -v $(pwd)/storage:/app/storage \
  -e IMG_STORE_API_KEY="your-api-key-here" \
  img-store
```

## Coolify Deployment

1. Push your code to GitHub
2. In Coolify dashboard:
   - Go to your application
   - Navigate to **Environment Variables**
   - Add variable:
     - **Name:** `IMG_STORE_API_KEY`
     - **Value:** Your API key from `.env`
     - **Secret:** ✓ (check this to hide from logs)
3. Deploy

## Security Best Practices

✅ `.env` is in `.gitignore` (never commit secrets)  
✅ Use environment variables in production  
✅ Mark as "Secret" in Coolify  
✅ Use HTTPS in production (configure reverse proxy)  
✅ Rotate API keys periodically  
✅ Different keys for dev/staging/prod  

## Testing Without Auth (Development Only)

If you don't set `IMG_STORE_API_KEY`, the server runs in public mode:
```bash
./build/bin/img-store
# ⚠️  WARNING: All endpoints are public!
```

This is useful for local testing but **NOT for production**.

## Response Codes

- `200` - Success
- `201` - Created (upload)
- `401` - Unauthorized (missing or invalid API key)
- `404` - Not found
- `500` - Server error

## Verification

All authentication tests passed ✓
- Public endpoints work without auth
- Protected endpoints reject requests without API key (401)
- Protected endpoints accept valid API key (Bearer or X-API-Key)
- Download remains public for CDN-like usage
