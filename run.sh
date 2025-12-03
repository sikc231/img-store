#!/bin/bash

# Start the img-store server with default or custom settings

PORT=${PORT:-8080}
STORAGE=${STORAGE:-./storage}

echo "Starting Image Storage Server..."
echo "Port: $PORT"
echo "Storage: $STORAGE"
echo ""

# Ensure storage directory exists
mkdir -p "$STORAGE"

# Start the server
./build/bin/img-store --port "$PORT" --storage "$STORAGE"
