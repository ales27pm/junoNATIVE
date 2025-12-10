// ============================================================

import React from "react";
import { View, StyleSheet } from "react-native";
import { useBottomTabBarHeight } from "@react-navigation/bottom-tabs";
import { useHeaderHeight } from "@react-navigation/elements";
import ControlPanel from "@/components/ControlPanel";
import Keyboard from "@/components/Keyboard";
import WaveformDisplay from "@/components/WaveformDisplay";
import { useSynth } from "@/contexts/SynthContext";
import { Colors, Spacing } from "@/constants/theme";
import { ThemedText } from "@/components/ThemedText";

export default function SynthesizerScreen() {
  const theme = Colors.dark;
  const headerHeight = useHeaderHeight();
  const tabBarHeight = useBottomTabBarHeight();
  const {
    params,
    switches,
    activeNotes,
    setParam,
    setSwitch,
    noteOn,
    noteOff,
    getCurrentPatch,
  } = useSynth();

  const currentPatch = getCurrentPatch();
  const isActive = activeNotes.size > 0;

  return (
    <View
      style={[
        styles.container,
        {
          backgroundColor: theme.backgroundRoot,
          paddingTop: headerHeight + Spacing.sm,
          paddingBottom: tabBarHeight,
        },
      ]}
    >
      <View style={styles.patchDisplay}>
        {currentPatch ? (
          <ThemedText style={[styles.patchName, { color: theme.text }]}>
            {currentPatch.name}
          </ThemedText>
        ) : (
          <ThemedText
            style={[styles.patchName, { color: theme.textSecondary }]}
          >
            No Patch Loaded
          </ThemedText>
        )}
      </View>

      <View style={styles.waveformContainer}>
        <WaveformDisplay
          isActive={isActive}
          waveType={
            switches.sawWaveOn ? "saw" : switches.pulseWaveOn ? "pulse" : "sine"
          }
        />
      </View>

      <View style={styles.controlsContainer}>
        <ControlPanel
          params={params}
          switches={switches}
          onParamChange={setParam}
          onSwitchChange={setSwitch}
        />
      </View>

      <View style={styles.keyboardContainer}>
        <Keyboard
          activeNotes={activeNotes}
          onNoteOn={noteOn}
          onNoteOff={noteOff}
          startOctave={3}
          numOctaves={2}
        />
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  patchDisplay: {
    paddingHorizontal: Spacing.lg,
    paddingVertical: Spacing.xs,
    alignItems: "center",
  },
  patchName: {
    fontSize: 14,
    fontWeight: "500",
    letterSpacing: 0.5,
  },
  waveformContainer: {
    paddingHorizontal: Spacing.lg,
    paddingVertical: Spacing.sm,
  },
  controlsContainer: {
    flex: 1,
    minHeight: 200,
  },
  keyboardContainer: {
    height: "35%",
    minHeight: 140,
  },
});

// ============================================================
