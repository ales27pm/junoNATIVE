#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_DIR="$ROOT_DIR"
OUTPUT_DIR="$ROOT_DIR/fastlane/build/codegen"

echo "[codegen] Root dir: $ROOT_DIR"
echo "[codegen] App dir:  $APP_DIR"
echo "[codegen] Output:   $OUTPUT_DIR"

if [ ! -d "$ROOT_DIR/node_modules" ]; then
  echo "[codegen] node_modules missing – installing dependencies..."
  cd "$ROOT_DIR"
  npm ci
else
  echo "[codegen] node_modules already present; skipping npm ci."
fi

mkdir -p "$OUTPUT_DIR"
ANDROID_APP_CODEGEN_DIR="$OUTPUT_DIR/android/app/build/generated/source/codegen"
mkdir -p "$ANDROID_APP_CODEGEN_DIR"
echo "[codegen] Ensured Android app codegen dir exists: $ANDROID_APP_CODEGEN_DIR"

echo "[codegen] Running React Native TurboModule codegen..."

export RCT_NEW_ARCH_ENABLED=1

node "$ROOT_DIR/node_modules/react-native/scripts/generate-specs-cli.js" \
  --path "$APP_DIR" \
  --outputPath "$OUTPUT_DIR/build" \
  --androidPath "$ANDROID_APP_CODEGEN_DIR" \
  --failOnWarn false

echo "[codegen] TurboModule artifacts generated at: $OUTPUT_DIR"
