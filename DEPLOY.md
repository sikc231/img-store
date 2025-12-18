# Coolify Deployment Configuration

This document provides instructions for deploying the Image Storage Server on Coolify.

## Prerequisites

- Coolify instance running
- GitHub repository connected to Coolify
- Domain or subdomain configured (optional)

## Deployment Steps

### 1. Create New Application

1. Go to your Coolify dashboard
2. Click "New Resource" → "Application"
3. Select your GitHub repository: `img-store`
4. Choose branch: `main`

### 2. Configure Build Settings

**Build Pack:** Docker (Dockerfile)

The project includes a multi-stage Dockerfile that will:
- Build the C++20 application with all dependencies
- Create a minimal runtime image
- Run as non-root user for security

### 3. Configure Persistent Storage

**Critical:** Mount a persistent volume to preserve images across deployments and restarts.

#### Option A: Using Coolify UI (Recommended)

1. Go to your application in Coolify
2. Navigate to **"Storages"** or **"Volumes"** tab
3. Click **"Add Volume"** or **"Add Persistent Storage"**
4. Configure:
   - **Name:** `img-store-data` (or any name)
   - **Source Path (Host):** `/var/lib/coolify/storage/img-store` (or your preferred host path)
   - **Destination Path (Container):** `/app/storage`
   - **Mount Type:** Bind mount or Volume
5. Click **"Save"** or **"Add"**

#### Option B: Using Docker Volume

If Coolify supports Docker volumes:
1. Go to "Volumes" section
2. Create a named volume: `img-store-data`
3. Mount it to: `/app/storage`

#### Option C: Manual Configuration (Advanced)

If you have access to the Coolify compose/Docker settings, ensure:
```yaml
volumes:
  - img-store-data:/app/storage

volumes:
  img-store-data:
    driver: local
```

⚠️ **Important:** Without persistent storage, all images will be lost when the container restarts!

### 4. Environment Variables (Optional)

No environment variables required for basic operation. Defaults:
- Port: `8080`
- Storage: `/app/storage` (mounted volume)

### 5. Port Configuration

- **Internal Port:** `8080`
- **External Port:** Auto-assigned by Coolify or configure custom port
- **Protocol:** HTTP

### 6. Health Check

The application includes a health endpoint:
- **Path:** `/health`
- **Method:** GET
- **Expected Response:** 200 OK with `{"status":"healthy"}`

Configure in Coolify:
- **Health Check Path:** `/health`
- **Health Check Interval:** 30s
- **Health Check Timeout:** 3s

### 7. Deploy

Click "Deploy" to start the build and deployment process.

## Post-Deployment

### Verify Deployment

```bash
# Check health
curl https://your-domain.com/health

# Upload test image
curl -X POST https://your-domain.com/images --data-binary @test.jpg

# Download image (use ID from upload response)
curl https://your-domain.com/images/<image-id> -o downloaded.jpg
```

### API Endpoints

- `GET /health` - Health check
- `POST /images` - Upload image (binary body)
- `GET /images/<id>` - Download image
- `DELETE /images/<id>` - Delete image

## Storage Persistence

### Important Notes

✅ **Images are preserved across deployments** when using persistent volumes
✅ **Automatic deduplication** - identical images share the same ID
✅ **Sharded storage** - efficient filesystem layout prevents directory overload

### Backup Strategy

Recommended backup approach:
```bash
# Backup storage directory
tar -czf img-store-backup-$(date +%Y%m%d).tar.gz /path/to/storage

# Restore
tar -xzf img-store-backup-YYYYMMDD.tar.gz -C /path/to/restore
```

## Scaling

The server is thread-safe and can handle concurrent requests. For horizontal scaling:
1. Use shared storage (NFS, S3-compatible, etc.)
2. Configure load balancer
3. Deploy multiple instances

## Monitoring

### Logs

Check Coolify logs for:
- Startup messages
- Request processing
- Error messages

### Metrics to Monitor

- HTTP response times
- Storage disk usage
- Request rate
- Error rate

## Troubleshooting

### Container Won't Start

- Check logs in Coolify
- Verify persistent volume is mounted correctly
- Ensure port 8080 is not blocked

### Images Lost After Redeployment

**This is the most common issue!** If images disappear after restart/redeploy:

1. **Check if volume is configured:**
   - In Coolify, go to your application → "Storages" or "Volumes" tab
   - Verify `/app/storage` is mounted to a persistent location

2. **Add persistent storage if missing:**
   - Follow Step 3 above to add persistent storage
   - Redeploy the application

3. **Verify volume is persistent (not ephemeral):**
   - The host path should be a real directory on your server
   - Example good paths: `/var/lib/coolify/storage/img-store`, `/data/img-store`
   - Example bad paths: `/tmp/storage` (temporary, will be deleted)

4. **Check volume permissions:**
   ```bash
   # SSH into your Coolify host
   sudo ls -la /var/lib/coolify/storage/img-store
   # Should show files owned by UID 1000 or imgstore user
   ```

5. **Verify data exists on host:**
   ```bash
   # Check if images are actually on the host
   find /var/lib/coolify/storage/img-store -type f | head -10
   ```

6. **Test after restart:**
   ```bash
   # Upload test image
   curl -X POST https://your-domain.com/images/named/test --data "test data"
   
   # Restart container in Coolify
   # Then check if image still exists
   curl https://your-domain.com/images/named/test
   ```

### Build Failures

- Check that all dependencies are available
- Review Dockerfile build logs
- Ensure sufficient build resources

## Security

- Application runs as non-root user (UID 1000)
- No sensitive data in environment variables
- HTTPS recommended for production (configure in Coolify)
- Consider adding authentication layer for write operations

## Updates

To update the application:
1. Push changes to GitHub `main` branch
2. Coolify will automatically rebuild and redeploy
3. Images in `/app/storage` are preserved
4. Zero-downtime deployment with Coolify's rolling updates
