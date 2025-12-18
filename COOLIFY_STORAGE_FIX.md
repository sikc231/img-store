# COOLIFY PERSISTENT STORAGE - Quick Fix Guide

## Problem: Images disappear after container restart

This happens when the `/app/storage` directory is not mounted to persistent storage.

## Solution: Add Persistent Volume in Coolify

### Step 1: Access Your Application in Coolify

1. Log into your Coolify dashboard
2. Go to your `img-store` application
3. Click on the application to open its settings

### Step 2: Add Persistent Storage

Look for one of these tabs (depends on Coolify version):
- **"Storages"**
- **"Volumes"**
- **"Persistent Storage"**
- **"Mounts"**

### Step 3: Create Volume Mapping

Click **"Add Storage"** or **"Add Volume"** and configure:

```
Source (Host Path):     /var/lib/coolify/storage/img-store
Destination (Container): /app/storage
Type:                   Bind Mount or Volume
```

Alternative host paths you can use:
- `/var/lib/coolify/storage/img-store`
- `/data/img-store`
- `/mnt/storage/img-store`
- Any persistent directory on your server

⚠️ **Important:** 
- Do NOT use `/tmp/` paths (they are temporary)
- Make sure the directory exists on your host
- Ensure proper permissions (owned by UID 1000 or writable by imgstore user)

### Step 4: Create Host Directory (if needed)

SSH into your Coolify host and create the directory:

```bash
# Create the storage directory
sudo mkdir -p /var/lib/coolify/storage/img-store

# Set ownership to UID 1000 (imgstore user in container)
sudo chown -R 1000:1000 /var/lib/coolify/storage/img-store

# Set permissions
sudo chmod -R 755 /var/lib/coolify/storage/img-store
```

### Step 5: Redeploy

1. Save the volume configuration in Coolify
2. Click **"Redeploy"** or **"Restart"**
3. Wait for the container to start

### Step 6: Verify It Works

```bash
# Upload a test image
curl -X POST https://your-domain.com/images/named/test-image \
  --data "This is a test image"

# Check it's there
curl https://your-domain.com/images/named/test-image
# Should return: "This is a test image"

# Restart the container in Coolify UI
# Then check again
curl https://your-domain.com/images/named/test-image
# Should STILL return: "This is a test image" ✅
```

## Coolify Version-Specific Instructions

### Coolify v4.x

1. Application → **Storages** tab
2. Click **"+ Add Persistent Storage"**
3. Fill in:
   - Name: `img-store-data`
   - Source: `/var/lib/coolify/storage/img-store`
   - Destination: `/app/storage`
4. Save and redeploy

### Coolify v3.x

1. Application → **Advanced** → **Volumes**
2. Add volume: `img-store-data:/app/storage`
3. Or use bind mount: `/var/lib/coolify/storage/img-store:/app/storage`
4. Save and redeploy

### Docker Compose Override (Alternative)

If Coolify allows docker-compose overrides, create a `docker-compose.override.yml`:

```yaml
version: '3.8'
services:
  img-store:
    volumes:
      - img-store-data:/app/storage

volumes:
  img-store-data:
    driver: local
```

## Verification Checklist

After adding persistent storage, verify:

- [ ] Volume is configured in Coolify UI
- [ ] Destination path is exactly `/app/storage`
- [ ] Host directory exists and has correct permissions
- [ ] Container has restarted after adding volume
- [ ] Test image persists after container restart
- [ ] No permission errors in container logs

## Troubleshooting

### Check if volume is mounted

```bash
# SSH to Coolify host
# Find your container ID
docker ps | grep img-store

# Inspect the container
docker inspect <container-id> | grep -A 10 "Mounts"

# Should show /app/storage mounted to a host path
```

### Check permissions

```bash
# On Coolify host
ls -la /var/lib/coolify/storage/img-store

# Should show directories and files owned by UID 1000
```

### Check container logs

In Coolify:
1. Go to your application
2. Check **"Logs"** tab
3. Look for errors like:
   - "Permission denied"
   - "Failed to create directory"
   - "Read-only file system"

### Test from inside container

```bash
# Get shell access to container
docker exec -it <container-id> bash

# Check storage directory
ls -la /app/storage

# Try to write
touch /app/storage/test.txt
# Should succeed without errors

# Check ownership
stat /app/storage
# Should show owner: imgstore (UID 1000)
```

## Common Mistakes

❌ **Using /tmp or ephemeral storage**
```
Source: /tmp/img-store  # WRONG - temporary directory
```

✅ **Using persistent location**
```
Source: /var/lib/coolify/storage/img-store  # CORRECT
```

---

❌ **Wrong container path**
```
Destination: /storage  # WRONG - app expects /app/storage
```

✅ **Correct container path**
```
Destination: /app/storage  # CORRECT - matches Dockerfile
```

---

❌ **Forgetting to redeploy**
- Added volume but didn't restart container

✅ **Always redeploy after adding volume**
- Changes only take effect after restart

## Still Having Issues?

1. **Check Coolify documentation** for your specific version
2. **Verify Dockerfile** declares volume: `VOLUME ["/app/storage"]` ✅
3. **Check host disk space**: `df -h`
4. **Review container logs** in Coolify
5. **Test locally** with docker-compose to verify it works
6. **Ask in Coolify Discord/Forum** with your configuration details

## Success Indicator

When properly configured, you should see:

```bash
# Storage directory on host has content
/var/lib/coolify/storage/img-store/
├── 24/
│   └── 38/
│       └── fd/
│           └── 5a594bbcb30660a5
├── names/
│   └── 6d/
│       └── 50/
│           └── 6b/
│               └── test-image.mapping
...
```

And images persist across container restarts! 🎉
