# Legacy separator markers

This repository historically used lines like `// ============================================================` as visual separators in a few files. These markers are fine in code that allows line comments (e.g., TypeScript, C++), but they break strict JSON parsers.

To avoid future `npm ci`/tooling failures, keep these separators **out of JSON-only files**. If you need to preserve the visual cue for a JSON document, add notes here instead of embedding comment markers in the JSON itself.

Recently cleaned files:
- `app.json`
- `Juno-Native/app.json`
