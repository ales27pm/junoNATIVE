# Agent Guidelines

These instructions apply to the entire repository.

## Workflow
- The primary app lives under `Juno-Native/`; top-level scripts proxy to that workspace.
- Use the pinned toolchain (`node >= 18`, `npm >= 11.4.2`) defined in the package manifests.
- Favor TypeScript/ESM patterns already used in `Juno-Native`. Avoid introducing stubs or partially implemented flows.

## Required checks
- Lint: `npm --prefix Juno-Native run lint`
- Tests: `npm --prefix Juno-Native run test`
- Formatting: `npm --prefix Juno-Native run format` (or `check:format` to verify without modifying files)
- Package metadata validation: `npm run validate:packages`

Run the relevant commands for any area you change. Document any skipped checks with a reason.

## Style and conventions
- Keep JSON files free of line-comment separator banners (`// =====`); see `docs/comment-separators.md` for context.
- Prefer Prettier defaults for JS/TS/JSON formatting. Avoid adding trailing debug logs.
- When modifying documentation, place new repo-wide notes in `docs/` unless the change belongs next to the code.

## Documentation references
- See `docs/codebase-overview.md` for a high-level map of client, synth engine, UI, audio, and server components.
- Legacy JSON separator guidance lives in `docs/comment-separators.md`.
