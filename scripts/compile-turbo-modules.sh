#!/usr/bin/env bash
set -euo pipefail

echo "[codegen] ====================================================="
echo "[codegen] TurboModule / Fabric codegen is handled by React Native's"
echo "[codegen] own Xcode/Pods build phases."
echo "[codegen] Skipping standalone codegen step in CI."
echo "[codegen] This script exists only so the Fastlane lane"
echo "[codegen] :compile_turbo_modules has a successful, deterministic step."
echo "[codegen] ====================================================="

# Optionally ensure the codegen output directory exists so any
# downstream tooling that expects it won't blow up.
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CODEGEN_DIR="$ROOT_DIR/fastlane/build/codegen"

mkdir -p "$CODEGEN_DIR"
echo "[codegen] Ensured codegen build directory exists at: $CODEGEN_DIR"

exit 0
