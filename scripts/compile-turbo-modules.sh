#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_DIR="$ROOT_DIR"
OUTPUT_DIR="$ROOT_DIR/fastlane/build/codegen"

echo "[codegen] Root dir: $ROOT_DIR"
echo "[codegen] App dir:  $APP_DIR"
echo "[codegen] Output:   $OUTPUT_DIR"

if [ ! -d "$APP_DIR/node_modules" ]; then
  echo "[codegen] node_modules not found. Installing JavaScript dependencies..."
  (cd "$ROOT_DIR" && npm ci)
else
  echo "[codegen] node_modules already present; skipping npm ci."
fi

CODEGEN_SCRIPT="$APP_DIR/node_modules/react-native/scripts/generate-codegen-artifacts.js"

if [ ! -f "$CODEGEN_SCRIPT" ]; then
  echo "[codegen] React Native codegen script not found at:"
  echo "          $CODEGEN_SCRIPT"
  echo "          Did you install React Native dependencies?"
  exit 1
fi

mkdir -p "$OUTPUT_DIR"

echo "[codegen] Running React Native TurboModule codegen..."
node "$CODEGEN_SCRIPT" \
  -p "$ROOT_DIR" \
  -t all \
  -o "$OUTPUT_DIR" \
  -s app

echo "[codegen] TurboModule artifacts generated at: $OUTPUT_DIR"
