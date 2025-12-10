// ============================================================

import React from "react";
import { View, StyleSheet, Image } from "react-native";
import { ThemedText } from "@/components/ThemedText";
import { Spacing, Typography } from "@/constants/theme";
import { useTheme } from "@/hooks/useTheme";

export default function HeaderTitle() {
  const { theme } = useTheme();

  return (
    <View style={styles.container}>
      <Image
        source={require("../../assets/images/icon.png")}
        style={styles.icon}
        resizeMode="contain"
      />
      <ThemedText style={[styles.title, { color: theme.text }]}>
        JUNO-106
      </ThemedText>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "flex-start",
  },
  icon: {
    width: 28,
    height: 28,
    marginRight: Spacing.sm,
    borderRadius: 6,
  },
  title: {
    ...Typography.patchName,
    letterSpacing: 1,
  },
});

// ============================================================
