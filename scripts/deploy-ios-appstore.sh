#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

usage() {
  cat <<'EOF'
Usage:
  scripts/deploy-ios-appstore.sh [--from-apple | --with-exported-cert] [--dry-run] [--skip-validations]

Modes:
  --from-apple         Create/fetch signing assets directly from Apple Developer portal,
                       then build and upload via Fastlane lane `ios automated_deploy_from_apple`.
  --with-exported-cert Use IOS_DIST_CERT_BASE64 + IOS_DIST_CERT_PASSWORD path,
                       then build and upload via Fastlane lane `ios local_build_and_submit`.

Flags:
  --dry-run            Validate environment and print deployment plan only.
  --skip-validations   Skip npm lint/package validation before Fastlane.

Required env (both modes):
  APP_IDENTIFIER
  APPLE_TEAM_ID
  APP_STORE_CONNECT_API_KEY_ID
  APP_STORE_CONNECT_API_KEY_ISSUER_ID
  APP_STORE_CONNECT_API_KEY_BASE64
  IOS_APPSTORE_PROFILE_NAME

Additional required env for --from-apple:
  APPLE_ID

Additional required env for --with-exported-cert:
  IOS_DIST_CERT_BASE64
  IOS_DIST_CERT_PASSWORD

Optional env:
  TESTFLIGHT_CHANGELOG
  IOS_SCHEME
  IOS_WORKSPACE
  IOS_OUTPUT_NAME
  IOS_SKIP_PODS=true|false
  IOS_FORCE_RENEW_CERTS=true|false
  IOS_FORCE_RENEW_PROFILE=true|false
EOF
}

require_env() {
  local var_name="$1"
  if [[ -z "${!var_name:-}" ]]; then
    echo "[deploy-ios-appstore] Missing required env: ${var_name}" >&2
    exit 1
  fi
}

print_plan() {
  cat <<EOF
[deploy-ios-appstore] Deployment plan
  mode: ${MODE}
  dry_run: ${DRY_RUN}
  run_validations: ${RUN_VALIDATIONS}
  fastlane_lane: ${LANE}
  app_identifier: ${APP_IDENTIFIER}
  team_id: ${APPLE_TEAM_ID}
  profile: ${IOS_APPSTORE_PROFILE_NAME}
  workspace: ${IOS_WORKSPACE:-ios/JunoNative.xcworkspace}
  scheme: ${IOS_SCHEME:-JunoNative}
EOF
}

MODE="--from-apple"
DRY_RUN=false
RUN_VALIDATIONS=true

while [[ $# -gt 0 ]]; do
  case "$1" in
    --from-apple|--with-exported-cert)
      MODE="$1"
      ;;
    --dry-run)
      DRY_RUN=true
      ;;
    --skip-validations)
      RUN_VALIDATIONS=false
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "[deploy-ios-appstore] Invalid argument: $1" >&2
      usage
      exit 1
      ;;
  esac
  shift
done

require_env APP_IDENTIFIER
require_env APPLE_TEAM_ID
require_env APP_STORE_CONNECT_API_KEY_ID
require_env APP_STORE_CONNECT_API_KEY_ISSUER_ID
require_env APP_STORE_CONNECT_API_KEY_BASE64
require_env IOS_APPSTORE_PROFILE_NAME

if [[ "$MODE" == "--from-apple" ]]; then
  require_env APPLE_ID
  LANE="ios automated_deploy_from_apple"
else
  require_env IOS_DIST_CERT_BASE64
  require_env IOS_DIST_CERT_PASSWORD
  LANE="ios local_build_and_submit"
fi

print_plan

cd "$ROOT_DIR"

echo "[deploy-ios-appstore] Verifying deploy environment"
bundle exec fastlane ios verify_deploy_environment

if [[ "$DRY_RUN" == true ]]; then
  echo "[deploy-ios-appstore] Dry run complete"
  exit 0
fi

if [[ "$RUN_VALIDATIONS" == true ]]; then
  echo "[deploy-ios-appstore] Running validations"
  npm run lint
  npm run validate:packages
fi

echo "[deploy-ios-appstore] Running Fastlane lane: ${LANE}"
bundle exec fastlane ${LANE}
