// ============================================================

import React from "react";
import { View, Pressable, StyleSheet, Platform } from "react-native";
import Animated, {
  useAnimatedStyle,
  withSpring,
} from "react-native-reanimated";
import * as Haptics from "expo-haptics";
import { ThemedText } from "@/components/ThemedText";
import { Colors, Spacing, Typography } from "@/constants/theme";

interface SynthSwitchProps {
  label: string;
  value: boolean;
  onChange: (value: boolean) => void;
}

export default function SynthSwitch({
  label,
  value,
  onChange,
}: SynthSwitchProps) {
  const theme = Colors.dark;

  const handlePress = () => {
    if (Platform.OS !== "web") {
      Haptics.impactAsync(Haptics.ImpactFeedbackStyle.Light);
    }
    onChange(!value);
  };

  const animatedTrackStyle = useAnimatedStyle(() => ({
    backgroundColor: withSpring(
      value ? theme.accent : theme.backgroundSecondary,
    ),
  }));

  const animatedKnobStyle = useAnimatedStyle(() => ({
    transform: [{ translateX: withSpring(value ? 16 : 0) }],
  }));

  return (
    <View style={styles.container}>
      <Pressable onPress={handlePress}>
        <Animated.View style={[styles.track, animatedTrackStyle]}>
          <Animated.View style={[styles.knob, animatedKnobStyle]} />
        </Animated.View>
      </Pressable>
      <ThemedText style={[styles.label, { color: theme.textSecondary }]}>
        {label}
      </ThemedText>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    alignItems: "center",
    gap: Spacing.xs,
  },
  track: {
    width: 36,
    height: 20,
    borderRadius: 10,
    padding: 2,
  },
  knob: {
    width: 16,
    height: 16,
    borderRadius: 8,
    backgroundColor: "#FFFFFF",
  },
  label: {
    ...Typography.knobLabel,
    textAlign: "center",
    marginTop: Spacing.xs,
  },
});

// ============================================================
