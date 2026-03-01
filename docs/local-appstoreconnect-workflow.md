# Local iOS Build + App Store Connect Submission (No EAS)

This project already supports a complete local release flow through Fastlane.

## Prerequisites

- macOS with Xcode + command line tools installed.
- Ruby + Bundler (`bundle install`).
- Node + npm (`npm ci`).
- CocoaPods (`bundle exec pod install --project-directory=ios` if needed).
- App Store Connect API key (`.p8`).
- Apple Distribution certificate (`.p12`) and password.
- App Store provisioning profile (name in Apple Developer portal).

## Required environment variables

```bash
export APP_IDENTIFIER="com.pulsr.junonative"
export APPLE_TEAM_ID="YOUR_TEAM_ID"

export APP_STORE_CONNECT_API_KEY_ID="ABC123XYZ"
export APP_STORE_CONNECT_API_KEY_ISSUER_ID="00000000-0000-0000-0000-000000000000"
# Can be raw key contents OR base64-encoded .p8 contents
export APP_STORE_CONNECT_API_KEY_BASE64="<P8_OR_BASE64_P8>"

export IOS_DIST_CERT_BASE64="<BASE64_OF_P12>"
export IOS_DIST_CERT_PASSWORD="<P12_PASSWORD>"
export IOS_APPSTORE_PROFILE_NAME="match AppStore com.pulsr.junonative"

# Optional
export IOS_SCHEME="JunoNative"
export IOS_WORKSPACE="ios/JunoNative.xcworkspace"
export IOS_OUTPUT_NAME="JunoNative.ipa"
export TESTFLIGHT_CHANGELOG="Local release build"
```

## 1) Validate JS/TS and package metadata

```bash
npm run lint
npm run validate:packages
```

## 2) End-to-end local build + upload (single command)

```bash
bundle exec fastlane ios local_build_and_submit
```

This lane will:

1. Import the distribution certificate.
2. Fetch/install provisioning profile with `sigh`.
3. Build C++ engine via CMake.
4. Generate TurboModules via absolute script path.
5. Install CocoaPods.
6. Archive + export App Store IPA to `fastlane/build/ios`.
7. Upload IPA to TestFlight via App Store Connect API key.

## 3) Run build and submit as separate steps (optional)

Build only:

```bash
bundle exec fastlane ios local_build_appstore_ipa
```

Submit the latest built IPA:

```bash
bundle exec fastlane ios submit_to_appstoreconnect
```

Submit a specific IPA path:

```bash
IOS_IPA_PATH="/absolute/path/to/JunoNative.ipa" bundle exec fastlane ios submit_to_appstoreconnect
```

## Troubleshooting

- **Missing ENV error**: ensure all required `export` values are set in the same shell session.
- **Signing errors**: verify `APPLE_TEAM_ID`, certificate password, and `IOS_APPSTORE_PROFILE_NAME` are correct.
- **Upload/auth errors**: verify API key ID, issuer ID, and `.p8` material.
- **Pods/codegen drift**: re-run `npm ci` and `bundle exec pod install --project-directory=ios`.

## Optional sideloading note (research/dev only)

For local device testing outside App Store Connect, use Xcode directly with a Development provisioning profile and run from `ios/JunoNative.xcworkspace` on a connected device.

## 4) Fully automated script (create/fetch credentials directly from Apple)

Use the project script to run validation + full deployment.

```bash
scripts/deploy-ios-appstore.sh --from-apple
# optional preflight only
scripts/deploy-ios-appstore.sh --from-apple --dry-run
```

This mode will call Fastlane lane `ios automated_deploy_from_apple`, which:

1. Uses `get_certificates` to create/fetch Apple Distribution cert material in your keychain.
2. Uses `sigh` with `readonly: false` to create/fetch the App Store provisioning profile.
3. Builds the IPA (`local_build_appstore_ipa`).
4. Uploads to TestFlight (`submit_to_appstoreconnect`).

> Apple Developer Portal operations (`get_certificates` / `sigh` with `readonly: false`) may require interactive Apple authentication or a valid `FASTLANE_SESSION` in headless shells.

Required extra variables for this mode:

```bash
export APPLE_ID="your-apple-id@example.com"
export IOS_APPSTORE_PROFILE_NAME="match AppStore com.pulsr.junonative"
# Optional renew flags
export IOS_FORCE_RENEW_CERTS="false"
export IOS_FORCE_RENEW_PROFILE="false"
```

## 5) Automated script with pre-exported certificate

If you already have a base64 `.p12` distribution cert:

```bash
scripts/deploy-ios-appstore.sh --with-exported-cert
```

This mode runs lane `ios local_build_and_submit`.

## 6) Script flags for safer automation

- `--dry-run`: validates env + toolchain and prints the selected lane without uploading.
- `--skip-validations`: skips `npm run lint` and `npm run validate:packages` (useful when these are already enforced in CI).

Examples:

```bash
# Preflight only
scripts/deploy-ios-appstore.sh --with-exported-cert --dry-run

# Full deploy using exported cert without re-running npm validations
scripts/deploy-ios-appstore.sh --with-exported-cert --skip-validations
```
