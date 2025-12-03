#!/bin/bash

# Setup script for API key authentication

set -e

echo "==========================================="
echo "  Image Store API Key Setup"
echo "==========================================="
echo ""

# Check if .env already exists
if [ -f .env ]; then
    echo "⚠️  .env file already exists!"
    read -p "Do you want to overwrite it? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Setup cancelled."
        exit 0
    fi
fi

# Check if openssl is available
if ! command -v openssl &> /dev/null; then
    echo "❌ openssl not found. Please install openssl to generate secure keys."
    exit 1
fi

# Generate a secure API key
echo "Generating secure API key..."
API_KEY=$(openssl rand -hex 32)

# Create .env file
cat > .env << EOF
# Image Storage Server Configuration
# Generated on $(date)

# API Key for write operations (POST/DELETE)
IMG_STORE_API_KEY=$API_KEY

# Server port (default: 8080)
# IMG_STORE_PORT=8080

# Storage directory (default: ./storage)
# IMG_STORE_STORAGE_DIR=/app/storage
EOF

echo "✅ .env file created successfully!"
echo ""
echo "==========================================="
echo "  Your API Key"
echo "==========================================="
echo ""
echo "$API_KEY"
echo ""
echo "⚠️  IMPORTANT: Keep this key secret!"
echo "   • Never commit .env to git"
echo "   • Use this key in your backend Authorization header"
echo "   • Store securely in Coolify environment variables"
echo ""
echo "==========================================="
echo "  Next Steps"
echo "==========================================="
echo ""
echo "1. Build the project:"
echo "   ./build.sh"
echo ""
echo "2. Start the server:"
echo "   ./build/bin/img-store"
echo ""
echo "3. Or use Docker:"
echo "   docker-compose up -d"
echo ""
echo "4. Test public endpoint (no auth needed):"
echo "   curl http://localhost:8080/health"
echo ""
echo "5. Test protected endpoint (requires API key):"
echo "   curl -X POST http://localhost:8080/images \\"
echo "     -H 'Authorization: Bearer $API_KEY' \\"
echo "     --data-binary @image.jpg"
echo ""
echo "For Coolify deployment:"
echo "  • Set IMG_STORE_API_KEY in environment variables"
echo "  • Mark it as 'Secret' to hide from logs"
echo ""
