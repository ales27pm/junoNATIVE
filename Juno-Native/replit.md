client/
  App.tsx                 - Main app entry with providers
  components/
    ControlPanel.tsx      - Synthesizer control panel with all knobs
    Knob.tsx              - Rotary knob component with gesture control
    Keyboard.tsx          - Touch-responsive piano keyboard
    SynthSwitch.tsx       - Toggle switch component
    ChorusSelector.tsx    - Chorus mode selector
    WaveformDisplay.tsx   - Animated waveform visualization
    PatchCard.tsx         - Patch browser card component
  contexts/
    SynthContext.tsx      - Shared synth state provider
  hooks/
    useSynthEngine.ts     - Core synth state and patch management
  screens/
    SynthesizerScreen.tsx - Main synth interface
    PatchesScreen.tsx     - Patch browser and management
    SettingsScreen.tsx    - App settings and configuration
  navigation/
    MainTabNavigator.tsx  - Tab navigation (Patches, Synth, Settings)
    RootStackNavigator.tsx - Root navigation stack
