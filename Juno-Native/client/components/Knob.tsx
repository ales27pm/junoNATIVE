// ============================================================

import React, { useCallback, useRef, useState } from "react";
import { View, StyleSheet, Platform } from "react-native";
import { Gesture, GestureDetector } from "react-native-gesture-handler";
import Animated, {
  useSharedValue,
  useAnimatedStyle,
  withSpring,
  runOnJS,
  interpolate,
} from "react-native-reanimated";
import * as Haptics from "expo-haptics";
import { ThemedText } from "@/components/ThemedText";
import { Colors, Spacing, Typography } from "@/constants/theme";

interface KnobProps {
  label: string;
  value: number;
  min?: number;
  max?: number;
  defaultValue?: number;
  onChange: (value: number) => void;
  size?: number;
  formatValue?: (value: number) => string;
}

export default function Knob({
  label,
  value,
  min = 0,
  max = 127,
  defaultValue,
  onChange,
  size = 56,
  formatValue,
}: KnobProps) {
  const theme = Colors.dark;
  const [isActive, setIsActive] = useState(false);
  const lastValue = useRef(value);
  const scale = useSharedValue(1);

  const normalizedValue = (value - min) / (max - min);
  const rotation = interpolate(normalizedValue, [0, 1], [-135, 135]);

  const triggerHaptic = useCallback(() => {
    if (Platform.OS !== "web") {
      Haptics.impactAsync(Haptics.ImpactFeedbackStyle.Light);
    }
  }, []);

  const handleValueChange = useCallback(
    (newValue: number) => {
      const clampedValue = Math.max(min, Math.min(max, Math.round(newValue)));
      if (clampedValue !== lastValue.current) {
        lastValue.current = clampedValue;
        onChange(clampedValue);
        if (clampedValue === min || clampedValue === max) {
          triggerHaptic();
        }
      }
    },
    [min, max, onChange, triggerHaptic]
  );

  const panGesture = Gesture.Pan()
    .onBegin(() => {
      scale.value = withSpring(1.1);
      runOnJS(setIsActive)(true);
    })
    .onUpdate((event) => {
      const sensitivity = 0.5;
      const delta = -event.translationY * sensitivity;
      const newValue = value + delta / 2;
      runOnJS(handleValueChange)(newValue);
    })
    .onEnd(() => {
      scale.value = withSpring(1);
      runOnJS(setIsActive)(false);
    });

  const tapGesture = Gesture.Tap()
    .numberOfTaps(2)
    .onEnd(() => {
      if (defaultValue !== undefined) {
        runOnJS(handleValueChange)(defaultValue);
        runOnJS(triggerHaptic)();
      }
    });

  const composedGesture = Gesture.Exclusive(tapGesture, panGesture);

  const animatedStyle = useAnimatedStyle(() => ({
    transform: [{ scale: scale.value }],
  }));

  const displayValue = formatValue
    ? formatValue(value)
    : value.toString();

  const arcLength = normalizedValue * 270;

  return (
    <View style={styles.container}>
      <GestureDetector gesture={composedGesture}>
        <Animated.View style={[styles.knobWrapper, animatedStyle]}>
          <View
            style={[
              styles.knobOuter,
              {
                width: size,
                height: size,
                borderRadius: size / 2,
                backgroundColor: theme.backgroundSecondary,
              },
            ]}
          >
            <View style={[styles.arcContainer, { width: size, height: size }]}>
              <View
                style={[
                  styles.arcTrack,
                  {
                    width: size - 4,
                    height: size - 4,
                    borderRadius: (size - 4) / 2,
                    borderColor: theme.knobTrack,
                  },
                ]}
              />
              {arcLength > 0 && (
                <View
                  style={[
                    styles.arcActive,
                    {
                      width: size - 4,
                      height: size - 4,
                      borderRadius: (size - 4) / 2,
                      borderColor: theme.accent,
                      transform: [
                        { rotate: "-135deg" },
                      ],
                    },
                  ]}
                />
              )}
            </View>
            <View
              style={[
                styles.knobInner,
                {
                  width: size - 12,
                  height: size - 12,
                  borderRadius: (size - 12) / 2,
                  backgroundColor: theme.backgroundTertiary,
                  transform: [{ rotate: `${rotation}deg` }],
                },
              ]}
            >
              <View
                style={[
                  styles.indicator,
                  { backgroundColor: isActive ? theme.accent : theme.text },
                ]}
              />
            </View>
          </View>
          {isActive && (
            <View style={[styles.valuePopup, { backgroundColor: theme.accent }]}>
              <ThemedText style={styles.valueText}>{displayValue}</ThemedText>
            </View>
          )}
        </Animated.View>
      </GestureDetector>
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
  knobWrapper: {
    position: "relative",
  },
  knobOuter: {
    alignItems: "center",
    justifyContent: "center",
    position: "relative",
  },
  arcContainer: {
    position: "absolute",
    alignItems: "center",
    justifyContent: "center",
  },
  arcTrack: {
    position: "absolute",
    borderWidth: 3,
    borderStyle: "solid",
  },
  arcActive: {
    position: "absolute",
    borderWidth: 3,
    borderStyle: "solid",
    borderTopColor: "transparent",
    borderRightColor: "transparent",
  },
  knobInner: {
    alignItems: "center",
    paddingTop: 4,
  },
  indicator: {
    width: 3,
    height: 10,
    borderRadius: 1.5,
  },
  valuePopup: {
    position: "absolute",
    top: -28,
    left: "50%",
    transform: [{ translateX: -20 }],
    paddingHorizontal: Spacing.sm,
    paddingVertical: Spacing.xs,
    borderRadius: 4,
    minWidth: 40,
    alignItems: "center",
  },
  valueText: {
    ...Typography.knobValue,
    color: "#FFFFFF",
  },
  label: {
    ...Typography.knobLabel,
    textAlign: "center",
    marginTop: Spacing.xs,
  },
});


// ============================================================
