# INGRES ChatBot - Multi-stage Docker Build
# Smart India Hackathon 2025

# Build stage for C components
FROM gcc:11 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    make \
    libpq-dev \
    libjson-c-dev \
    libcurl4-openssl-dev \
    postgresql-client \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY src/ ./src/
COPY include/ ./include/
COPY CMakeLists.txt .
COPY Makefile .

# Build the C application
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Runtime stage
FROM node:18-alpine AS runtime

# Install runtime dependencies
RUN apk add --no-cache \
    postgresql-client \
    curl \
    && npm install -g vercel

# Create app directory
WORKDIR /app

# Copy built C binary
COPY --from=builder /app/build/bin/ingres_chatbot ./bin/ingres_chatbot

# Copy web files
COPY web/ ./web/
COPY api/ ./api/
COPY package.json .
COPY vercel.json .
COPY deploy_vercel.sh .

# Install Node.js dependencies
RUN npm install

# Create non-root user
RUN addgroup -g 1001 -S nodejs && \
    adduser -S nextjs -u 1001

# Change ownership
RUN chown -R nextjs:nodejs /app
USER nextjs

# Expose port
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/api/health || exit 1

# Default command
CMD ["npm", "start"]