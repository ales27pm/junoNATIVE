// ============================================================

import React, { useEffect } from "react";
import { View, StyleSheet } from "react-native";
import Animated, {
  useSharedValue,
  useAnimatedStyle,
  withRepeat,
  withTiming,
  Easing,
  withSpring,
} from "react-native-reanimated";
import { Colors, Spacing } from "@/constants/theme";

interface WaveformDisplayProps {
  isActive: boolean;
  waveType?: "sine" | "saw" | "pulse" | "noise";
}

export default function WaveformDisplay({
  isActive,
  waveType = "saw",
}: WaveformDisplayProps) {
  const theme = Colors.dark;
  const animationPhase = useSharedValue(0);
  const opacity = useSharedValue(0.3);

  useEffect(() => {
    if (isActive) {
      animationPhase.value = withRepeat(
        withTiming(1, { duration: 2000, easing: Easing.linear }),
        -1,
        false,
      );
      opacity.value = withSpring(1);
    } else {
      opacity.value = withSpring(0.3);
    }
  }, [animationPhase, isActive, opacity]);

  const animatedContainerStyle = useAnimatedStyle(() => ({
    opacity: opacity.value,
  }));

  const bars = Array.from({ length: 24 }, (_, i) => {
    const getSawHeight = (index: number) => {
      const cycle = (index % 8) / 8;
      return 20 + cycle * 60;
    };

    const getPulseHeight = (index: number) => {
      const cycle = index % 8;
      return cycle < 4 ? 70 : 20;
    };

    const getNoiseHeight = () => {
      return 20 + Math.random() * 60;
    };

    let height: number;
    switch (waveType) {
      case "sine":
        height = 40 + Math.sin((i / 24) * Math.PI * 4) * 30;
        break;
      case "saw":
        height = getSawHeight(i);
        break;
      case "pulse":
        height = getPulseHeight(i);
        break;
      case "noise":
        height = getNoiseHeight();
        break;
      default:
        height = getSawHeight(i);
    }

    return (
      <View
        key={i}
        style={[
          styles.bar,
          {
            height,
            backgroundColor: theme.waveform,
          },
        ]}
      />
    );
  });

  return (
    <Animated.View
      style={[
        styles.container,
        { backgroundColor: theme.backgroundSecondary },
        animatedContainerStyle,
      ]}
    >
      <View style={styles.waveformContainer}>{bars}</View>
    </Animated.View>
  );
}

const styles = StyleSheet.create({
  container: {
    height: 60,
    borderRadius: 8,
    overflow: "hidden",
    justifyContent: "center",
    paddingHorizontal: Spacing.sm,
  },
  waveformContainer: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-between",
    height: "100%",
    gap: 2,
  },
  bar: {
    flex: 1,
    borderRadius: 1,
  },
});

// ============================================================
