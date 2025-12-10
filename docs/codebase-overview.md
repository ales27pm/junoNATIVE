# Codebase Overview

## Client application structure
- **Entry & providers:** `client/index.js` registers the root `App` component. `App.tsx` wraps navigation in error boundaries, React Query, Safe Area, gesture, keyboard, and synthesizer context providers, then renders the navigation tree and status bar.
- **Navigation:** `RootStackNavigator` hosts a single `MainTabNavigator` screen. The tab navigator defines three tabs—Patches, Synthesizer, and Settings—applies theme-aware tab colors, iOS blur backgrounds, and shared stack screen options from `useScreenOptions` (transparent headers, platform-aware backgrounds, and gesture configuration).

## Synth engine state & workflows
- **State container:** `SynthContext` exposes synth parameters, switches, patch list, active notes, and CRUD helpers backed by the `useSynthEngine` hook. Consumers use `SynthProvider` to access the hook across the app.
- **Patch lifecycle:** On mount, `useSynthEngine` loads patches from AsyncStorage, seeding factory presets if none exist, and restores the last selected patch. Loading a patch synchronizes parameters/switches and persists the active patch id. Saving creates a timestamped patch snapshot of current params/switches; deleting removes it and clears the active selection if needed.
- **Parameter management:** `setParam` clamps MIDI-style values to 0–127; `setSwitch` toggles boolean or enumerated switches. `useEffect` pushes parameter and switch updates to the Web Audio synth when running on web.
- **Performance state:** `noteOn` and `noteOff` mutate an `activeNotes` set (supporting multi-touch keyboard feedback) and forward the events to the Web Audio engine on web builds.

## UI surfaces & interaction patterns
- **Synthesizer screen:** Lays out patch title, animated waveform meter, horizontal control panel, and a 2-octave keyboard. Padding respects header/tab heights.
- **Control panel:** Groups knobs for LFO, DCO, VCF, VCA, ENV sections plus switches for oscillator footings, waveforms, and VCF/VCA modes. Includes a chorus mode selector and uses themed dividers for visual grouping.
- **Knob interaction model:** Vertical pan adjusts values with clamping and spring scaling; double-tap resets to defaults when provided. Active arcs/indicators show position, while haptics (non-web) trigger on boundary hits or reset.
- **Keyboard:** Generates two octaves of pressable keys, computes positions for black keys, triggers haptics on press, and tracks active note highlighting until release.
- **Patch browser:** Provides search, category chips, grid of patch cards, long-press delete with platform-specific confirmation, and a floating action button to open a “Save Patch” modal that captures name/category before persisting via the synth engine.
- **Settings:** Presents static sections for audio, MIDI, appearance, and links (GitHub/documentation), using icon-labeled rows with optional chevrons.

## Web Audio synthesis pipeline (web builds)
- **Initialization:** On web platforms, `WebAudioSynth` creates an `AudioContext`, master gain, LFO oscillator/gain, and stereo chorus delays with modulating LFOs. Parameters and switches are cached for voice updates.
- **Parameter propagation:** `updateParams` maps UI values to audio nodes—e.g., LFO rate modulation, chorus speed per mode, and per-voice filter frequency/Q adjustments using the stored params/switches.
- **Voice handling:** `noteOn` resumes the audio context if suspended, enforces six-voice polyphony by stealing the oldest note, and creates oscillators per enabled waveform/footing plus optional sub-oscillator. Signals route through a low-pass filter, optional chorus wet/dry mix, and master gain. ADSR-style amplitude envelopes derive from UI params and input velocity. `noteOff` schedules release ramps and cleans up oscillators/nodes after the release interval; `allNotesOff`/`dispose` stop and disconnect all resources.

## Server scaffold
- **Express host:** `server/index.ts` wires CORS for configured Replit domains, JSON/urlencoded parsing (capturing raw bodies), and request logging for `/api` paths. It serves Expo static assets with manifest routing based on the `expo-platform` header and renders a templated landing page that injects dynamic host/protocol values.
- **HTTP server:** `registerRoutes` currently returns an HTTP server with an empty `/api` surface, ready for future expansion. Error handling middleware serializes error responses and rethrows for visibility.

## Supporting utilities
- **Query client:** `lib/query-client.ts` builds a React Query client with a helper for composing API base URLs from `EXPO_PUBLIC_DOMAIN`, a fetch wrapper that throws on non-OK responses, and a configurable unauthorized behavior for queries.
- **Theming & typography:** `constants/theme.ts` centralizes dark-mode-first colors, spacing, radii, typography scales, platform fonts, and shadow presets used across components.
