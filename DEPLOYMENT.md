# Coolify Deployment Guide

This guide explains how to deploy the img-store server on Coolify with persistent storage.

## ⚠️ CRITICAL: Persistent Storage Required

**Without persistent storage, all images will be lost when the container restarts!**

See [COOLIFY_STORAGE_FIX.md](COOLIFY_STORAGE_FIX.md) for detailed troubleshooting if images disappear.

## Quick Deploy

1. **Push to GitHub**
   ```bash
   git add .
   git commit -m "Complete image storage server"
   git push origin main
   ```

2. **In Coolify Dashboard**
   - Click "New Resource" → "Application"
   - Select your GitHub repository: `sikc231/img-store`
   - Branch: `main`
   - Build Pack: `Dockerfile`

3. **Configure Persistent Storage** ⚠️ **REQUIRED**
   
   In the application settings:
   - Go to **"Storages"** or **"Volumes"** tab
   - Click **"Add Volume"** or **"Add Persistent Storage"**
   - Configure:
     - **Source (Host)**: `/var/lib/coolify/storage/img-store` (or your preferred path)
     - **Destination (Container)**: `/app/storage` (must be exactly this)
     - **Type**: Bind Mount or Volume
   
   This ensures images survive deployments and restarts! 🎯

4. **Environment Variables** (optional)
   - `PORT`: Default is 8080
   - No other environment variables needed

5. **Port Configuration**
   - The application exposes port 8080
   - Coolify will automatically map it to your domain

## Important: Persistent Storage

⚠️ **Critical**: Without persistent storage configuration, images will be deleted on each deployment!

### Storage Configuration Options:

#### Option 1: Named Volume (Recommended)
```yaml
# In Coolify, this creates a persistent Docker volume
Container Path: /app/storage
Volume Name: img-store-data
```

#### Option 2: Bind Mount
```yaml
# Maps to a specific host directory
Container Path: /app/storage
Host Path: /var/lib/coolify/storage/img-store
```

## API Endpoints

Once deployed, your server will be available at your Coolify domain:

```bash
# Health check
GET https://your-domain.com/health

# Upload image
POST https://your-domain.com/images
Content-Type: application/octet-stream
Body: <binary image data>

# Download image
GET https://your-domain.com/images/{image-id}

# Delete image
DELETE https://your-domain.com/images/{image-id}
```

## Testing Your Deployment

```bash
# Upload an image
curl -X POST https://your-domain.com/images \
  --data-binary @photo.jpg

# Response will include the image ID:
# {"id":"a1b2c3d4e5f6g7h8","status":"uploaded","size":12345}

# Download the image
curl https://your-domain.com/images/a1b2c3d4e5f6g7h8 -o downloaded.jpg

# Delete the image
curl -X DELETE https://your-domain.com/images/a1b2c3d4e5f6g7h8
```

## Automatic Rebuilds

Coolify automatically rebuilds when you push to `main`:

1. Make changes to your code
2. `git push origin main`
3. Coolify detects the push and rebuilds
4. **Images in `/app/storage` are preserved** (if volume is configured)
5. New version is deployed seamlessly

## Health Checks

Coolify can use the built-in health check:
- **Endpoint**: `/health`
- **Method**: GET
- **Expected Response**: 200 OK with `{"status":"healthy",...}`

## Troubleshooting

### Images disappear after deployment
- **Problem**: Storage not configured as persistent
- **Solution**: Add persistent volume in Coolify storage settings

### Build fails
- **Check**: CMake and build logs in Coolify
- **Common issue**: Missing dependencies (should be in Dockerfile)

### Server not responding
- **Check**: Port 8080 is exposed
- **Check**: Health endpoint returns 200

## Scaling

The server is multithreaded and can handle concurrent requests. For horizontal scaling:
- Use a shared network filesystem for `/app/storage`
- Or implement S3-compatible storage backend
- Configure load balancer in front of multiple instances

## Monitoring

Watch the logs in Coolify for:
- Upload/download activity
- Error messages
- Storage usage

Consider setting up alerts for:
- Disk space usage on storage volume
- HTTP error rates
- Response time degradation
