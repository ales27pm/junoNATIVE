# Legacy separator markers

This repository historically used lines like:

```text
// ============================================================
```

as visual separators inside JSON and other configuration files.

These are fine in languages that allow comments (e.g. TypeScript, C++),
but they break strict JSON parsers and some tooling (npm ci, pod install, etc.).

To avoid future tooling failures:
- Do not embed decorative separator comment lines inside JSON files.
- Keep such separators in Markdown docs, code comments, or this file instead.

Recently cleaned files:
- app.json
- package.json
