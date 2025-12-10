// ============================================================

# JUNO-106 Synthesizer Design Guidelines

## Architecture Decisions

### Authentication
**No Authentication Required**
- This is a single-user music production tool with local patch storage
- Include a Settings screen with:
  - User preferences (audio buffer size, MIDI settings, theme)
  - App info and credits
  - No user avatar needed (utility app)

### Navigation
**Tab Navigation (3 tabs)**
- **Synthesizer** (center) - Main synthesis interface with all controls
- **Patches** (left) - Patch browser and management
- **Settings** (right) - Audio/MIDI configuration and preferences

Position: Core action (play/perform) is on the center tab (Synthesizer)

### Screen Specifications

#### 1. Synthesizer Screen (Main Performance Interface)
**Purpose:** Real-time synthesis control and keyboard performance

**Layout:**
- **Header:** Transparent with current patch name centered
  - Left: Menu button (⋯) for patch save/load
  - Right: Audio monitor (VU meter visualization)
  - No search bar
  - Top inset: headerHeight + Spacing.xl
- **Main Content:** Non-scrollable (fixed layout)
  - Control panel area (upper 60% of screen)
  - Keyboard area (lower 40% of screen)
- **Safe Area Insets:**
  - Top: headerHeight + Spacing.xl (transparent header)
  - Bottom: tabBarHeight + Spacing.xl

**Components:**
- 16 rotary knobs organized in sections (LFO, DCO, VCF, VCA, ENV)
- 11 toggle switches for oscillator/effect routing
- Touch-responsive 2-octave keyboard (24 keys)
- Real-time audio waveform visualization above keyboard
- Patch name display with "JUNO-106" branding

#### 2. Patches Screen (Patch Browser)
**Purpose:** Browse, load, and manage synthesis presets

**Layout:**
- **Header:** Default navigation header
  - Title: "Patches"
  - Right: Import button (↓) for .106 files
  - Search bar integrated in header
- **Main Content:** Scrollable grid
  - 2-column grid of patch cards
  - Each card shows patch name, category icon, and preview waveform
- **Safe Area Insets:**
  - Top: Spacing.xl (non-transparent header)
  - Bottom: tabBarHeight + Spacing.xl

**Components:**
- Search bar with real-time filtering
- Category filter chips (Basses, Leads, Pads, FX)
- Patch cards with visual waveform previews
- Long-press for patch management (rename, duplicate, delete)

#### 3. Settings Screen
**Purpose:** Configure audio engine and app preferences

**Layout:**
- **Header:** Default navigation header, title: "Settings"
- **Main Content:** Scrollable form with grouped sections
- **Safe Area Insets:**
  - Top: Spacing.xl
  - Bottom: tabBarHeight + Spacing.xl

**Components:**
- Audio Engine section (buffer size, sample rate)
- MIDI Configuration section (channel, velocity curve)
- Appearance section (dark/light mode toggle)
- About section (version, credits, open-source licenses)

## Design System

### Color Palette
**Inspired by vintage analog synthesizers with modern refinement**

Primary Colors:
- **Surface:** #1A1A1A (dark charcoal, like vintage synth panel)
- **Surface Elevated:** #2A2A2A
- **Accent Orange:** #FF6B35 (warm, reminiscent of 1980s LED displays)
- **Accent Blue:** #00BCD4 (for active parameters)

Functional Colors:
- **Text Primary:** #FFFFFF
- **Text Secondary:** #B0B0B0
- **Knob Track:** #3A3A3A
- **Knob Active:** Linear gradient from Accent Orange to Accent Blue
- **Key White:** #F5F5F5
- **Key Black:** #1A1A1A
- **Key Pressed:** #FF6B35 with 80% opacity
- **Waveform:** #00BCD4 with 60% opacity

### Typography
- **Patch Names:** SF Pro Display, 18pt, Semibold
- **Knob Labels:** SF Pro Text, 10pt, Regular, Letter-spacing: 0.5pt, Uppercase
- **Knob Values:** SF Mono, 12pt, Medium (for precise numerical display)
- **Section Headers:** SF Pro Text, 14pt, Bold, Uppercase
- **Body Text:** SF Pro Text, 16pt, Regular

### Visual Design

**Control Aesthetics:**
- Rotary knobs inspired by vintage hardware:
  - Circular design with radial indicator line
  - Dark metallic appearance (#2A2A2A base with subtle gradient)
  - Touch feedback: subtle glow effect on active knob
  - Value arc displays in Accent Orange
  - Haptic feedback on value changes (light impact)
- Toggle switches:
  - Physical rocker switch aesthetic
  - Clear ON/OFF states with LED-style indicators
  - Smooth animation (200ms) between states

**Keyboard Design:**
- Authentic piano key proportions (white keys 7:1 ratio, black keys slightly raised)
- White keys with subtle gradient (#F5F5F5 to #E0E0E0)
- Black keys with semi-transparent shadow for depth
- Active key highlight: Inner glow in Accent Orange
- Velocity sensitivity visualized by glow intensity
- Haptic feedback on key press (light impact)

**Icons:**
- System icons from SF Symbols for iOS
- Feather icons from @expo/vector-icons for cross-platform consistency
- NO emojis in the interface

**Shadows:**
- Keyboard keys: Subtle elevation
  - White keys: shadowOffset (0, 1), shadowOpacity 0.1, shadowRadius 2
  - Black keys: shadowOffset (0, 2), shadowOpacity 0.3, shadowRadius 3
- Floating elements (if any): shadowOffset (0, 2), shadowOpacity 0.10, shadowRadius 2

### Interaction Design

**Knob Control:**
- Vertical drag to adjust value (up = increase, down = decrease)
- Double-tap to reset to default value
- Value display appears above knob during interaction
- Smooth parameter interpolation to prevent audio clicks
- Visual feedback: Active arc fills as value increases

**Keyboard:**
- Multi-touch support for polyphonic playing (6 voices max)
- Press velocity mapped from touch force (iOS) or touch area (Android)
- Visual feedback: immediate color change on touch
- Key release: smooth fade-out animation (100ms)
- Sustain pedal support via external MIDI controller

**Patch Browser:**
- Tap card to load patch instantly
- Pull-to-refresh for file system rescan
- Swipe actions: Left swipe reveals delete, right swipe reveals duplicate
- Loading animation during .106 file parsing

### Accessibility Requirements

**Audio Production Accessibility:**
- VoiceOver support with descriptive labels for all knobs ("VCF Cutoff: 1200 Hz")
- High contrast mode support (increased knob indicator visibility)
- Haptic feedback for parameter boundaries (prevents over-turning)
- Clear visual indicators for audio clipping/overload
- Adjustable keyboard key size (Settings)

**Color Blindness:**
- Orange/Blue color scheme chosen for deuteranopia compatibility
- Waveform visualization uses brightness variation in addition to color
- All critical information conveyed through shape and position, not just color

### Critical Assets

**Generated Assets (Required for Core Experience):**
1. **Waveform Visualizations** (5 assets):
   - Sawtooth wave preview
   - Pulse wave preview
   - Sub-oscillator wave preview
   - Triangle LFO wave preview
   - Filtered output wave preview
   - Style: Clean vector graphics in Accent Blue (#00BCD4)

2. **App Icon:**
   - Stylized JUNO-106 front panel representation
   - Dominant colors: Dark charcoal with orange accent
   - Recognizable synthesizer silhouette

3. **Patch Category Icons** (4 assets):
   - Bass (low frequency waveform)
   - Lead (sharp sawtooth)
   - Pad (soft, layered waves)
   - FX (chaotic, modulated pattern)
   - Monochromatic, simple line art

**NO overuse of custom imagery** - Rely on system icons for navigation and standard actions.

### Layout Patterns

**Control Panel Organization:**
- 5 logical sections arranged left-to-right:
  1. LFO (2 knobs)
  2. DCO (4 knobs + 3 switches)
  3. VCF (5 knobs + 1 switch)
  4. VCA (1 knob + 1 switch)
  5. ENV (4 knobs)
- Section dividers: Subtle vertical lines (#3A3A3A, 1px)
- Section labels above knobs

**Responsive Scaling:**
- Landscape orientation: Keyboard height reduces to 30%, control panel expands
- Portrait orientation: Standard 60/40 split
- iPad: Side-by-side layout with Patches browser visible alongside Synthesizer

### Performance Considerations
- 60 FPS knob animations (use `useNativeDriver: true`)
- Throttled parameter updates (max 60 updates/sec to DSP engine)
- Lazy-loaded patch thumbnails
- Memoized waveform visualizations


// ============================================================
