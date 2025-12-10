# Codebase Overview (High-Level)

This document summarizes the major areas of the project so new contributors can orient quickly. It is intentionally concise to reduce maintenance overhead; rely on in-file comments and tests for up-to-date, API-level detail.

## Client application
- React Native entry (`client/index.js`) bootstraps the `App` component with navigation, query, synthesizer, safe-area, and gesture providers.
- Navigation uses a root stack that presents a tab navigator with three primary surfaces: **Patches**, **Synthesizer**, and **Settings**. Shared screen options handle platform-aware headers, backgrounds, and gestures.

## Synth engine
- `SynthContext` and `useSynthEngine` expose synthesizer parameters, switches, patch storage, and note events to the UI.
- Patches are persisted in AsyncStorage (factory presets seeded when none exist). Selecting, saving, or deleting a patch keeps parameters/switches and the last active patch id in sync.
- Web builds forward parameter/switch updates and note on/off events to the Web Audio synth wrapper.

## UI surfaces
- **Synthesizer:** Control panels of knobs/switches plus a multi-octave keyboard with clamped value changes, reset gestures, and haptics.
- **Patch browser:** Searchable grid with create/delete flows wired to the synth engine.
- **Settings:** Static sections for audio/MIDI/display preferences and external links.

## Web Audio pipeline (web targets)
- `WebAudioSynth` initializes the audio context, master routing, LFO/chorus modulation, and per-voice oscillators/filters.
- Enforces bounded polyphony, resumes audio context on demand, and schedules envelope release/cleanup on note-off.

## Server scaffold
- Express host with CORS and asset serving for Expo clients, plus a placeholder `/api` surface ready for expansion.

## Supporting utilities
- React Query client helpers, theming primitives, and HTTP wrappers that compose API base URLs and normalize fetch errors.
- **HTTP server:** `registerRoutes` currently returns an HTTP server with an initialized `/api` surface (e.g., with logging middleware) but no functional routes, ready for future expansion. Error handling middleware serializes error responses and rethrows for visibility.
> For implementation specifics or edge-case behaviors, prefer reading the source (components, hooks, and utilities) where TSDoc/JSDoc and tests are maintained alongside the code.
