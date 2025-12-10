// ============================================================

import { Platform } from "react-native";

export const Colors = {
  light: {
    text: "#FFFFFF",
    textSecondary: "#B0B0B0",
    buttonText: "#FFFFFF",
    tabIconDefault: "#687076",
    tabIconSelected: "#FF6B35",
    link: "#00BCD4",
    backgroundRoot: "#1A1A1A",
    backgroundDefault: "#2A2A2A",
    backgroundSecondary: "#3A3A3A",
    backgroundTertiary: "#404040",
    accent: "#FF6B35",
    accentBlue: "#00BCD4",
    knobTrack: "#3A3A3A",
    keyWhite: "#F5F5F5",
    keyBlack: "#1A1A1A",
    keyPressed: "rgba(255, 107, 53, 0.8)",
    waveform: "rgba(0, 188, 212, 0.6)",
    divider: "#3A3A3A",
    success: "#4CAF50",
    warning: "#FFC107",
    error: "#F44336",
  },
  dark: {
    text: "#FFFFFF",
    textSecondary: "#B0B0B0",
    buttonText: "#FFFFFF",
    tabIconDefault: "#9BA1A6",
    tabIconSelected: "#FF6B35",
    link: "#00BCD4",
    backgroundRoot: "#1A1A1A",
    backgroundDefault: "#2A2A2A",
    backgroundSecondary: "#3A3A3A",
    backgroundTertiary: "#404040",
    accent: "#FF6B35",
    accentBlue: "#00BCD4",
    knobTrack: "#3A3A3A",
    keyWhite: "#F5F5F5",
    keyBlack: "#1A1A1A",
    keyPressed: "rgba(255, 107, 53, 0.8)",
    waveform: "rgba(0, 188, 212, 0.6)",
    divider: "#3A3A3A",
    success: "#4CAF50",
    warning: "#FFC107",
    error: "#F44336",
  },
};

export const Spacing = {
  xs: 4,
  sm: 8,
  md: 12,
  lg: 16,
  xl: 20,
  "2xl": 24,
  "3xl": 32,
  "4xl": 40,
  "5xl": 48,
  inputHeight: 48,
  buttonHeight: 52,
};

export const BorderRadius = {
  xs: 8,
  sm: 12,
  md: 18,
  lg: 24,
  xl: 30,
  "2xl": 40,
  "3xl": 50,
  full: 9999,
};

export const Typography = {
  patchName: {
    fontSize: 18,
    fontWeight: "600" as const,
  },
  knobLabel: {
    fontSize: 10,
    fontWeight: "400" as const,
    letterSpacing: 0.5,
    textTransform: "uppercase" as const,
  },
  knobValue: {
    fontSize: 12,
    fontWeight: "500" as const,
  },
  sectionHeader: {
    fontSize: 14,
    fontWeight: "700" as const,
    textTransform: "uppercase" as const,
  },
  h1: {
    fontSize: 32,
    fontWeight: "700" as const,
  },
  h2: {
    fontSize: 28,
    fontWeight: "700" as const,
  },
  h3: {
    fontSize: 24,
    fontWeight: "600" as const,
  },
  h4: {
    fontSize: 20,
    fontWeight: "600" as const,
  },
  body: {
    fontSize: 16,
    fontWeight: "400" as const,
  },
  small: {
    fontSize: 14,
    fontWeight: "400" as const,
  },
  link: {
    fontSize: 16,
    fontWeight: "400" as const,
  },
};

export const Fonts = Platform.select({
  ios: {
    sans: "system-ui",
    serif: "ui-serif",
    rounded: "ui-rounded",
    mono: "ui-monospace",
  },
  default: {
    sans: "normal",
    serif: "serif",
    rounded: "normal",
    mono: "monospace",
  },
  web: {
    sans: "system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif",
    serif: "Georgia, 'Times New Roman', serif",
    rounded:
      "'SF Pro Rounded', 'Hiragino Maru Gothic ProN', Meiryo, 'MS PGothic', sans-serif",
    mono: "SFMono-Regular, Menlo, Monaco, Consolas, 'Liberation Mono', 'Courier New', monospace",
  },
});

export const Shadows = {
  whiteKey: {
    shadowColor: "#000",
    shadowOffset: { width: 0, height: 1 },
    shadowOpacity: 0.1,
    shadowRadius: 2,
    elevation: 2,
  },
  blackKey: {
    shadowColor: "#000",
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.3,
    shadowRadius: 3,
    elevation: 4,
  },
  card: {
    shadowColor: "#000",
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.1,
    shadowRadius: 2,
    elevation: 2,
  },
};

// ============================================================
