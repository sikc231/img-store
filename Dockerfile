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
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -u 1000 imgstore && \
    mkdir -p /app/storage && \
    chown -R imgstore:imgstore /app

WORKDIR /app

# Copy binary from builder
COPY --from=builder /app/build/bin/img-store /app/img-store

# Set ownership
RUN chown imgstore:imgstore /app/img-store

# Switch to non-root user
USER imgstore

# Expose port
EXPOSE 8080

# Create volume mount point
VOLUME ["/app/storage"]

# Environment variable for API key (set this in .env or Coolify)
ENV IMG_STORE_API_KEY=""

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD wget --no-verbose --tries=1 --spider http://localhost:8080/health || exit 1

# Run the server (API key from environment variable)
CMD ["/app/img-store", "--port", "8080", "--storage", "/app/storage"]
