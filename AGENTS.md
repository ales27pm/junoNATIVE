# Agent Guidelines

These instructions apply to automated agents and human contributors working on the JunoNative project.

## Project layout overview

The current layout is:

- `App.tsx` / `index.js` – React Native entrypoint
- `src/` – modern UI and control surface (keyboard, knobs, patch browser, etc.)
- `rtn-juno-engine/` – C++ DSP core + iOS / Android bridges
- `ios/` – iOS Xcode project and Pod configuration
- `android/` – Android Gradle project
- `tests/` – C++ DSP and integration tests
- `.github/workflows/` – CI/CD configuration
- `fastlane/` – deployment automation

The primary React Native app now lives at the **repository root**.
The legacy `Juno-Native/` workspace is deprecated and can be removed.

## Agent responsibilities

1. **Do not resurrect `Juno-Native/` paths**

   - All tooling, scripts, and workflows must assume the React Native app is rooted at the repository root.
   - Do not add `--prefix Juno-Native` or `Juno-Native/…` paths to new commands.

2. **Use root-level npm commands**

   - Install dependencies at the root:
     - `npm ci`
   - Lint:
     - `npm run lint`
   - Validate package files:
     - `npm run validate:packages`

3. **C++ DSP tests**

   - Tests are driven via CMake:
     - `cmake -S . -B build`
     - `cmake --build build`
     - `ctest --test-dir build --output-on-failure -C Debug`

4. **Codegen**

   - TurboModule / RN codegen is invoked via:
     - `bash scripts/compile-turbo-modules.sh`

5. **JSON / comment separators**

   - Do not place decorative separator lines inside JSON files.
   - Instead, see the guidance in `docs/comment-separators.md`.

This is the canonical layout; any automated refactors should preserve it.
