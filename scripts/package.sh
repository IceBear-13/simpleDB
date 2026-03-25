#!/bin/bash
set -e

VERSION="1.1.0"
INSTALL_DIR="simpledbms-${VERSION}"

if [ ! -d "$INSTALL_DIR" ]; then
  echo "Error: Run build.sh first"
  exit 1
fi

# Create archives
tar -czf "simpledbms-${VERSION}-linux-x86_64.tar.gz" "$INSTALL_DIR"
tar -cf "simpledbms-${VERSION}.tar" \
    --exclude='.git' \
    --exclude='build' \
    --exclude='.cache' \
    --exclude='.vscode' \
    .

gzip "simpledbms-${VERSION}.tar"

echo "✓ Packaged:"
echo "  - simpledbms-${VERSION}-linux-x86_64.tar.gz (Binary)"
echo "  - simpledbms-${VERSION}.tar.gz (Source)"