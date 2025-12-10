// ============================================================

import React from "react";
import { View, Pressable, StyleSheet } from "react-native";
import { Feather } from "@expo/vector-icons";
import { ThemedText } from "@/components/ThemedText";
import { Colors, Spacing, Typography, BorderRadius, Shadows } from "@/constants/theme";
import { Patch } from "@/hooks/useSynthEngine";

interface PatchCardProps {
  patch: Patch;
  isActive: boolean;
  onPress: () => void;
  onLongPress?: () => void;
}

const categoryIcons: Record<Patch["category"], keyof typeof Feather.glyphMap> = {
  bass: "activity",
  lead: "zap",
  pad: "cloud",
  fx: "wind",
};

const categoryColors: Record<Patch["category"], string> = {
  bass: "#FF6B35",
  lead: "#00BCD4",
  pad: "#9C27B0",
  fx: "#4CAF50",
};

export default function PatchCard({
  patch,
  isActive,
  onPress,
  onLongPress,
}: PatchCardProps) {
  const theme = Colors.dark;
  const iconName = categoryIcons[patch.category];
  const categoryColor = categoryColors[patch.category];

  return (
    <Pressable
      onPress={onPress}
      onLongPress={onLongPress}
      style={({ pressed }) => [
        styles.container,
        {
          backgroundColor: theme.backgroundDefault,
          borderColor: isActive ? theme.accent : "transparent",
        },
        pressed && styles.pressed,
        Shadows.card,
      ]}
    >
      <View style={[styles.categoryBadge, { backgroundColor: categoryColor + "30" }]}>
        <Feather name={iconName} size={20} color={categoryColor} />
      </View>
      <ThemedText style={[styles.name, { color: theme.text }]} numberOfLines={1}>
        {patch.name}
      </ThemedText>
      <ThemedText
        style={[styles.category, { color: theme.textSecondary }]}
      >
        {patch.category.toUpperCase()}
      </ThemedText>
      {isActive && (
        <View style={[styles.activeIndicator, { backgroundColor: theme.accent }]} />
      )}
    </Pressable>
  );
}

const styles = StyleSheet.create({
  container: {
    padding: Spacing.lg,
    borderRadius: BorderRadius.sm,
    borderWidth: 2,
    gap: Spacing.sm,
    position: "relative",
    overflow: "hidden",
  },
  pressed: {
    opacity: 0.8,
    transform: [{ scale: 0.98 }],
  },
  categoryBadge: {
    width: 40,
    height: 40,
    borderRadius: 20,
    alignItems: "center",
    justifyContent: "center",
    marginBottom: Spacing.xs,
  },
  name: {
    ...Typography.patchName,
  },
  category: {
    ...Typography.knobLabel,
  },
  activeIndicator: {
    position: "absolute",
    top: 0,
    left: 0,
    right: 0,
    height: 3,
  },
});


// ============================================================
