#!/usr/bin/env bash
# This script is a no-op in CI. TurboModule codegen is handled by Xcode/Pod phases.
set -e

echo "[codegen] ====================================================="
echo "[codegen] TurboModule / Fabric codegen is handled by React Native's"
echo "[codegen] own Xcode/Pods build phases."
echo "[codegen] Skipping standalone codegen step in CI."
echo "[codegen] This script exists only so the Fastlane lane"
echo "[codegen] :compile_turbo_modules has a successful, deterministic step."
echo "[codegen] ====================================================="

OUTPUT_DIR="$(pwd)/fastlane/build/codegen"
mkdir -p "$OUTPUT_DIR"
echo "[codegen] Ensured codegen build directory exists at: $OUTPUT_DIR"
