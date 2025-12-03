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

**Critical:** Mount a persistent volume to preserve images across deployments.

In Coolify:
1. Go to "Storage" section
2. Add a new volume:
   - **Name:** `img-store-data`
   - **Mount Path:** `/app/storage`
   - **Source:** Choose persistent volume location on your server

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

- Verify persistent volume configuration
- Check mount path is `/app/storage`
- Ensure volume source is persistent (not ephemeral)

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
