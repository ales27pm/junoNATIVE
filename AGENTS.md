# Agent & Contributor Guidelines

These instructions apply to **automated agents** (e.g. AI coding assistants, bots) and **human contributors** working on the JunoNative project.

The goal is that any change you make is:

- **Deterministic** – same inputs → same outputs
- **Observable** – effects are visible in CI logs, artifacts, or tests
- **Minimal** – smallest possible change that solves the problem
- **Aligned** – respects the existing architecture and tooling

---

## 1. Repository layout (canonical map)

Treat this as the source of truth for “where things live”:

- `App.tsx` / `index.js`  
  React Native app entrypoint.

- `src/`  
  Modern UI and control surface (keyboard, knobs, patch browser, etc.).

- `rtn-juno-engine/`  
  C++ DSP core and platform bridges:
  - `cpp/` – platform–agnostic DSP code, tests, and CMake configuration.
  - `ios/` – iOS-specific DSP / render integration (e.g. Metal-based engine).
  - `android/` – Android-specific integration and shared library project.

- `ios/`  
  iOS Xcode project and CocoaPods configuration:
  - `JunoNative.xcodeproj` – **static library** target producing `libJunoNative.a`.
  - `JunoNative.xcworkspace` – workspace used for builds.
  - `Podfile` – React Native + Hermes + autolinking configuration.

- `android/`  
  Gradle-based Android project for the native engine.

- `tests/`  
  C++ DSP and integration tests (driven via CMake/CTest).

- `.github/workflows/`  
  CI/CD configuration for:
  - C++ engine builds and tests
  - iOS simulator build (static lib)
  - iOS TestFlight / release via Fastlane
  - (Optionally) Android CI

- `fastlane/`  
  Deployment automation:
  - `Fastfile` – lanes for `beta`, `release`, and shared helpers.
  - `fastlane/build/` – CMake build dir, iOS artifacts, etc.

- `scripts/`  
  Project-level utility scripts:
  - `compile-turbo-modules.sh` – TurboModule / React Native codegen.
  - `validate-package-json.js` – package metadata validation, etc.

- `docs/`  
  Additional documentation. In particular:
  - `docs/comment-separators.md` – how to structure comments & separators.

This layout is **canonical**. Any automated refactors must preserve it, unless a change is explicitly requested.

---

## 2. Tooling overview

### 2.1 JavaScript / React Native

- Package manager: `npm` (see `package.json`).
- Engines:
  - Node: `>= 18`
  - npm: `>= 11.4.2`

Common scripts:

- `npm run start` – Metro bundler.
- `npm run ios` – Run app on iOS simulator (when an app target exists).
- `npm run android` – Run app on Android.
- `npm run lint` – ESLint across the repo.
- `npm run validate:packages` – Validate package metadata.

When modifying JS/TS code:

- Keep imports relative to `src/` or aliases already in use.
- Prefer functional components and hooks where the codebase already does.
- Keep changes **local and minimal**; avoid bulk reformatting.

---

### 2.2 C++ DSP engine (CMake)

The C++ engine and tests live in `rtn-juno-engine/` with a root CMake configuration at the repository root.

Canonical build & test commands (from repo root):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure -C Debug

Guidelines:
	•	Keep platform-agnostic DSP logic in rtn-juno-engine/cpp/ where possible.
	•	Use compile-time guards (#if, TARGET_OS_IPHONE, etc.) to separate platform-specific paths.
	•	Do not introduce dependencies on Apple or Android SDKs in generic C++ code.
	•	When adding tests, integrate them via CMake and ensure ctest remains green.

⸻

2.3 Codegen / TurboModules

TurboModule / RN codegen is driven by:

bash scripts/compile-turbo-modules.sh

This script:
	•	Runs from any working directory by resolving the repo root internally.
	•	Should be invoked by CI and Fastlane via an absolute path, e.g.:

sh("bash #{File.join(ROOT_DIR, 'scripts/compile-turbo-modules.sh')}")

When modifying TurboModule definitions:
	•	Update the relevant JS/TS specs.
	•	Re-run the script locally before committing.
	•	Ensure generated files are deterministic (no timestamps, no machine-specific paths).

⸻

3. iOS: Static library and CI artifacts

3.1 What the iOS target actually builds

The Xcode project ios/JunoNative.xcodeproj defines a static library target:
	•	Target name: JunoNative
	•	Product type: com.apple.product-type.library.static
	•	Output: libJunoNative.a (not a .app)

Do not assume this repo builds a standalone iOS app. It produces a library used by an app.

3.2 GitHub Actions – iOS simulator build

The iOS CI workflow (in .github/workflows/build_ios.yml) performs:
	1.	xcodebuild
	2.	Runs a deterministic find command to locate libJunoNative.a
	3.	Uploads the artifact

This ensures CI remains robust even when Xcode changes output folder naming conventions.

When editing this workflow:
	•	Preserve the find + cp pattern.
	•	Validate success by checking logs and uploaded artifacts.

⸻

4. iOS: Fastlane & TestFlight

Fastlane configuration lives in fastlane/Fastfile.

Key constants:

ROOT_DIR = File.expand_path("..", __dir__)
IOS_OUTPUT_DIR     = File.join(ROOT_DIR, "fastlane/build/ios")
CMAKE_BUILD_DIR    = File.join(ROOT_DIR, "fastlane/build/cmake")
CMAKE_CONFIGURATION = "Release"

4.1 Critical rule for TurboModules

Always invoke:

sh("bash #{File.join(ROOT_DIR, 'scripts/compile-turbo-modules.sh')}")

Never:

sh("bash scripts/compile-turbo-modules.sh")

Because Fastlane may change the working directory.

4.2 Build flow for TestFlight
	1.	Build C++ engine via CMake
	2.	Compile TurboModules
	3.	Build Swift/iOS code
	4.	Export + upload via App Store Connect API key

⸻

5. JSON, comments, and separators
	•	Avoid decorative separators (//////, =====) especially inside JSON.
	•	Keep documentation machine-readable.
	•	See docs/comment-separators.md for formatting conventions.

⸻

6. General rules for agents (AI and human)
	1.	Respect conventions
Keep diffs minimal and consistent.
	2.	Make changes observable
CI logs must clearly show what changed or what failed.
	3.	Preserve determinism
Avoid machine-specific paths, timestamps, randomness.
	4.	Do not remove required build steps
If removing or replacing steps, update workflows + this doc.
	5.	Remember: this repo currently builds a static library
Not an app.
CI artifacts should reflect this unless the architecture changes.
