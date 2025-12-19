# Fix Permission Issue in Coolify

## The Problem

Error: `Permission denied [/app/storage/f9/cb/89]`

The container user (UID 1000) cannot create subdirectories in the mounted storage volume.

## Solution: Fix Permissions on Coolify Server

### Step 1: SSH into your Coolify server

```bash
ssh your-server
```

### Step 2: Find your storage mount point

Check your Coolify application's volume configuration. Common paths:
- `/var/lib/coolify/storage/img-store`
- `/data/img-store`
- Check your volume mount in Coolify UI

### Step 3: Fix ownership and permissions

```bash
# Replace with your actual storage path from Coolify
STORAGE_PATH="/var/lib/coolify/storage/img-store"

# Set ownership to UID 1000 (the imgstore user in container)
sudo chown -R 1000:1000 "$STORAGE_PATH"

# Set permissions to allow writing
sudo chmod -R 775 "$STORAGE_PATH"

# Verify
ls -la "$STORAGE_PATH"
```

### Step 4: Verify the permissions

```bash
# Should show:
# drwxrwxr-x ... 1000 1000 ... storage-directory
stat -c "%u:%g %a" "$STORAGE_PATH"
# Should output: 1000:1000 775
```

### Step 5: Restart container in Coolify

Go to Coolify UI and click **Restart** on your img-store application.

### Step 6: Test

```bash
# Upload a test image
curl -X POST https://your-domain.com/images/named/test-permission \
  -H "Authorization: Bearer your-api-key" \
  --data-binary "test data"

# Should work now without permission errors
```

## Alternative: Run Container as Root (Not Recommended)

If you cannot change host permissions, you can temporarily modify the Dockerfile:

```dockerfile
# Comment out the USER line at the end of Dockerfile
# USER imgstore

# Run as root (security risk, not recommended for production)
```

Then rebuild in Coolify.

## Best Practice Solution

Add this to your Coolify deployment script or commands:

```bash
# Pre-deployment hook (if Coolify supports it)
mkdir -p /var/lib/coolify/storage/img-store
chown -R 1000:1000 /var/lib/coolify/storage/img-store
chmod -R 775 /var/lib/coolify/storage/img-store
```
