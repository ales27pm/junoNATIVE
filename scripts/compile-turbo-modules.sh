#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_DIR="$ROOT_DIR/Juno-Native"
OUTPUT_DIR="$ROOT_DIR/fastlane/build/codegen"

if [ ! -d "$APP_DIR/node_modules" ]; then
  echo "Installing JavaScript dependencies for TurboModule codegen..."
  npm ci --prefix "$APP_DIR"
fi

mkdir -p "$OUTPUT_DIR"

node "$APP_DIR/node_modules/react-native/scripts/generate-codegen-artifacts.js" \
  -p "$APP_DIR" \
  -t all \
  -o "$OUTPUT_DIR" \
  -s app
