// ============================================================

import React, { useCallback } from "react";
import { View, StyleSheet, Pressable, Platform } from "react-native";
import * as Haptics from "expo-haptics";
import { Colors, Shadows } from "@/constants/theme";

interface KeyboardProps {
  activeNotes: Set<number>;
  onNoteOn: (note: number, velocity: number) => void;
  onNoteOff: (note: number) => void;
  startOctave?: number;
  numOctaves?: number;
}

const WHITE_KEY_NOTES = [0, 2, 4, 5, 7, 9, 11];
const BLACK_KEY_OFFSETS = [0.7, 1.7, 3.7, 4.7, 5.7];

export default function Keyboard({
  activeNotes,
  onNoteOn,
  onNoteOff,
  startOctave = 3,
  numOctaves = 2,
}: KeyboardProps) {
  const theme = Colors.dark;

  const triggerHaptic = useCallback(() => {
    if (Platform.OS !== "web") {
      Haptics.impactAsync(Haptics.ImpactFeedbackStyle.Light);
    }
  }, []);

  const handleKeyPress = useCallback(
    (note: number) => {
      triggerHaptic();
      onNoteOn(note, 100);
    },
    [onNoteOn, triggerHaptic],
  );

  const handleKeyRelease = useCallback(
    (note: number) => {
      onNoteOff(note);
    },
    [onNoteOff],
  );

  const renderOctave = (octave: number, octaveIndex: number) => {
    const baseNote = octave * 12;
    const whiteKeyWidth = 100 / (numOctaves * 7);
    const blackKeyWidth = whiteKeyWidth * 0.6;

    return (
      <View key={octave} style={styles.octave}>
        {WHITE_KEY_NOTES.map((offset, index) => {
          const note = baseNote + offset;
          const isActive = activeNotes.has(note);

          return (
            <Pressable
              key={`white-${note}`}
              onPressIn={() => handleKeyPress(note)}
              onPressOut={() => handleKeyRelease(note)}
              style={({ pressed }) => [
                styles.whiteKey,
                {
                  backgroundColor:
                    isActive || pressed ? theme.keyPressed : theme.keyWhite,
                  width: `${whiteKeyWidth}%`,
                },
                Shadows.whiteKey,
              ]}
            />
          );
        })}
        {BLACK_KEY_OFFSETS.map((offsetPos, index) => {
          const blackNote = baseNote + [1, 3, 6, 8, 10][index];
          const isActive = activeNotes.has(blackNote);
          const leftPosition = (octaveIndex * 7 + offsetPos) * whiteKeyWidth;

          return (
            <Pressable
              key={`black-${blackNote}`}
              onPressIn={() => handleKeyPress(blackNote)}
              onPressOut={() => handleKeyRelease(blackNote)}
              style={({ pressed }) => [
                styles.blackKey,
                {
                  backgroundColor:
                    isActive || pressed ? theme.keyPressed : theme.keyBlack,
                  width: `${blackKeyWidth}%`,
                  left: `${leftPosition}%`,
                },
                Shadows.blackKey,
              ]}
            />
          );
        })}
      </View>
    );
  };

  const octaves = Array.from({ length: numOctaves }, (_, i) => startOctave + i);

  return (
    <View style={styles.container}>
      <View style={styles.keyboard}>
        {octaves.map((octave, index) => renderOctave(octave, index))}
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    width: "100%",
    height: "100%",
  },
  keyboard: {
    flex: 1,
    flexDirection: "row",
    position: "relative",
  },
  octave: {
    flex: 1,
    flexDirection: "row",
    position: "relative",
  },
  whiteKey: {
    flex: 1,
    height: "100%",
    borderWidth: 1,
    borderColor: "#E0E0E0",
    borderRadius: 0,
    borderBottomLeftRadius: 4,
    borderBottomRightRadius: 4,
  },
  blackKey: {
    position: "absolute",
    height: "60%",
    borderBottomLeftRadius: 4,
    borderBottomRightRadius: 4,
    zIndex: 1,
  },
});

// ============================================================
