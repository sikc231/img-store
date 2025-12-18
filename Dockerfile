# Multi-stage build for C++20 image storage server
FROM debian:bookworm-slim AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libxxhash-dev \
    libboost-system-dev \
    libasio-dev \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY CMakeLists.txt .
COPY include/ include/
COPY src/ src/

# Build the project
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

# Runtime stage
FROM debian:bookworm-slim

# Install runtime dependencies only
RUN apt-get update && apt-get install -y \
    libxxhash0 \
    libstdc++6 \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user and group
RUN groupadd -g 1000 imgstore && \
    useradd -m -u 1000 -g 1000 imgstore

WORKDIR /app

# Copy binary from builder
COPY --from=builder /app/build/bin/img-store /app/img-store

# Create storage directory with proper permissions BEFORE declaring volume
RUN mkdir -p /app/storage && \
    chown -R 1000:1000 /app/storage && \
    chmod -R 755 /app/storage && \
    chown 1000:1000 /app/img-store && \
    chmod 755 /app/img-store

# Expose port
EXPOSE 8080

# Label for Coolify port detection
LABEL coolify.port="8080"

# Declare volume AFTER setting permissions
VOLUME ["/app/storage"]

# Switch to non-root user AFTER all permission setup
USER imgstore

# Environment variable for API key (set this in .env or Coolify)
ENV IMG_STORE_API_KEY=""

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

# Run the server (API key from environment variable)
CMD ["/app/img-store", "--port", "8080", "--storage", "/app/storage"]
