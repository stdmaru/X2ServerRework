
#!/usr/bin/env bash

set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"

# Default distribution directory
DIST_DIR="${1:-$REPO_ROOT/dist}"

# Resolve relative destination paths against the repository root
if [[ "$DIST_DIR" != /* ]]; then
    DIST_DIR="$REPO_ROOT/$DIST_DIR"
fi

BUILD_DIR="$REPO_ROOT/build/linux-release/run"
COMPOSE_FILE="$SCRIPT_DIR/docker-compose.yml"
DOCKERFILE="$SCRIPT_DIR/server/Dockerfile"

log() {
    echo "[INFO] $*"
}

error() {
    echo "[ERROR] $*" >&2
    exit 1
}

# Check required commands
for cmd in docker cp mkdir; do
    command -v "$cmd" >/dev/null 2>&1 \
        || error "Required command not found: $cmd"
done

# Validate required source files and directories
[[ -d "$BUILD_DIR" ]] \
    || error "Build directory not found: $BUILD_DIR"

[[ -f "$COMPOSE_FILE" ]] \
    || error "Docker Compose file not found: $COMPOSE_FILE"

[[ -f "$DOCKERFILE" ]] \
    || error "Dockerfile not found: $DOCKERFILE"

# Check Docker availability
docker info >/dev/null 2>&1 \
    || error "Docker is unavailable. Check that the daemon is running and accessible."

log "Preparing distribution directory: $DIST_DIR"

mkdir -p "$DIST_DIR" \
    || error "Failed to create distribution directory: $DIST_DIR"

# Copy Docker configuration files
log "Copying Docker configuration files..."

cp "$COMPOSE_FILE" "$DOCKERFILE" "$DIST_DIR/" \
    || error "Failed to copy Docker configuration files."

# Copy application binaries and resources
log "Copying application build artifacts..."

cp -a "$BUILD_DIR/." "$DIST_DIR/" \
    || error "Failed to copy build artifacts."

# Create control directory
log "Creating control directory..."

mkdir -p "$DIST_DIR/control" \
    || error "Failed to create control directory."

# Build Docker image
log "Building Docker image: x2-server:local"

docker build --no-cache \
    --build-arg HOST_UID="$(id -u)" \
    --build-arg HOST_GID="$(id -g)" \
    -t x2-server:local \
    "$DIST_DIR" \
    || error "Docker image build failed."

log "Distribution prepared successfully."
log "Output directory: $DIST_DIR"
log "Docker image: x2-server:local"
