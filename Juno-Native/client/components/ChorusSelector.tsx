// ============================================================

import React from "react";
import { View, Pressable, StyleSheet, Platform } from "react-native";
import * as Haptics from "expo-haptics";
import { ThemedText } from "@/components/ThemedText";
import { Colors, Spacing, Typography, BorderRadius } from "@/constants/theme";

interface ChorusSelectorProps {
  value: 0 | 1 | 2;
  onChange: (value: 0 | 1 | 2) => void;
}

export default function ChorusSelector({ value, onChange }: ChorusSelectorProps) {
  const theme = Colors.dark;

  const handlePress = (newValue: 0 | 1 | 2) => {
    if (Platform.OS !== "web") {
      Haptics.impactAsync(Haptics.ImpactFeedbackStyle.Light);
    }
    onChange(newValue);
  };

  return (
    <View style={styles.container}>
      <View style={[styles.selectorRow, { backgroundColor: theme.backgroundSecondary }]}>
        {[0, 1, 2].map((mode) => (
          <Pressable
            key={mode}
            onPress={() => handlePress(mode as 0 | 1 | 2)}
            style={[
              styles.option,
              value === mode && { backgroundColor: theme.accent },
            ]}
          >
            <ThemedText
              style={[
                styles.optionText,
                { color: value === mode ? "#FFFFFF" : theme.textSecondary },
              ]}
            >
              {mode === 0 ? "OFF" : mode === 1 ? "I" : "II"}
            </ThemedText>
          </Pressable>
        ))}
      </View>
      <ThemedText style={[styles.label, { color: theme.textSecondary }]}>
        CHORUS
      </ThemedText>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    alignItems: "center",
    gap: Spacing.xs,
  },
  selectorRow: {
    flexDirection: "row",
    borderRadius: BorderRadius.xs,
    overflow: "hidden",
  },
  option: {
    paddingHorizontal: Spacing.md,
    paddingVertical: Spacing.sm,
  },
  optionText: {
    ...Typography.knobLabel,
    fontWeight: "600",
  },
  label: {
    ...Typography.knobLabel,
    textAlign: "center",
    marginTop: Spacing.xs,
  },
});


// ============================================================
