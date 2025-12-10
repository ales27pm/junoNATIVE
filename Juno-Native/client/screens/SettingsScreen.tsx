// ============================================================

import React from "react";
import {
  View,
  StyleSheet,
  ScrollView,
  Pressable,
  Linking,
  Platform,
} from "react-native";
import { useBottomTabBarHeight } from "@react-navigation/bottom-tabs";
import { Feather } from "@expo/vector-icons";
import { ThemedText } from "@/components/ThemedText";
import { Colors, Spacing, Typography, BorderRadius } from "@/constants/theme";

interface SettingItemProps {
  icon: keyof typeof Feather.glyphMap;
  label: string;
  value?: string;
  onPress?: () => void;
}

function SettingItem({ icon, label, value, onPress }: SettingItemProps) {
  const theme = Colors.dark;

  return (
    <Pressable
      onPress={onPress}
      style={({ pressed }) => [
        styles.settingItem,
        { backgroundColor: theme.backgroundDefault },
        pressed && onPress && styles.settingItemPressed,
      ]}
    >
      <View
        style={[
          styles.settingIcon,
          { backgroundColor: theme.backgroundSecondary },
        ]}
      >
        <Feather name={icon} size={20} color={theme.accent} />
      </View>
      <ThemedText style={[styles.settingLabel, { color: theme.text }]}>
        {label}
      </ThemedText>
      {value ? (
        <ThemedText
          style={[styles.settingValue, { color: theme.textSecondary }]}
        >
          {value}
        </ThemedText>
      ) : null}
      {onPress ? (
        <Feather name="chevron-right" size={20} color={theme.textSecondary} />
      ) : null}
    </Pressable>
  );
}

interface SettingSectionProps {
  title: string;
  children: React.ReactNode;
}

function SettingSection({ title, children }: SettingSectionProps) {
  const theme = Colors.dark;

  return (
    <View style={styles.section}>
      <ThemedText style={[styles.sectionTitle, { color: theme.textSecondary }]}>
        {title}
      </ThemedText>
      <View style={styles.sectionContent}>{children}</View>
    </View>
  );
}

export default function SettingsScreen() {
  const theme = Colors.dark;
  const tabBarHeight = useBottomTabBarHeight();

  return (
    <ScrollView
      style={[styles.container, { backgroundColor: theme.backgroundRoot }]}
      contentContainerStyle={[
        styles.content,
        { paddingTop: Spacing.xl, paddingBottom: tabBarHeight + Spacing.xl },
      ]}
      showsVerticalScrollIndicator={false}
    >
      <SettingSection title="Audio Engine">
        <SettingItem icon="cpu" label="Sample Rate" value="44.1 kHz" />
        <SettingItem icon="layers" label="Buffer Size" value="256 samples" />
        <SettingItem icon="volume-2" label="Polyphony" value="6 voices" />
      </SettingSection>

      <SettingSection title="MIDI">
        <SettingItem icon="radio" label="MIDI Channel" value="All" />
        <SettingItem icon="sliders" label="Velocity Curve" value="Linear" />
      </SettingSection>

      <SettingSection title="Appearance">
        <SettingItem icon="moon" label="Theme" value="Dark" />
        <SettingItem icon="maximize-2" label="Display" value="Compact" />
      </SettingSection>

      <SettingSection title="About">
        <SettingItem icon="info" label="Version" value="1.0.0" />
        <SettingItem
          icon="github"
          label="Source Code"
          onPress={() => {
            if (Platform.OS !== "web") {
              Linking.openURL("https://github.com");
            } else {
              window.open("https://github.com", "_blank");
            }
          }}
        />
        <SettingItem icon="book" label="Documentation" onPress={() => {}} />
      </SettingSection>

      <View style={styles.footer}>
        <ThemedText style={[styles.footerText, { color: theme.textSecondary }]}>
          JUNO-106 Synthesizer
        </ThemedText>
        <ThemedText
          style={[styles.footerSubtext, { color: theme.textSecondary }]}
        >
          Inspired by the classic Roland JUNO-106
        </ThemedText>
      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  content: {
    paddingHorizontal: Spacing.lg,
    gap: Spacing.xl,
  },
  section: {
    gap: Spacing.sm,
  },
  sectionTitle: {
    ...Typography.knobLabel,
    marginLeft: Spacing.xs,
    marginBottom: Spacing.xs,
  },
  sectionContent: {
    borderRadius: BorderRadius.sm,
    overflow: "hidden",
    gap: 1,
  },
  settingItem: {
    flexDirection: "row",
    alignItems: "center",
    padding: Spacing.md,
    gap: Spacing.md,
  },
  settingItemPressed: {
    opacity: 0.7,
  },
  settingIcon: {
    width: 36,
    height: 36,
    borderRadius: 8,
    alignItems: "center",
    justifyContent: "center",
  },
  settingLabel: {
    flex: 1,
    ...Typography.body,
  },
  settingValue: {
    ...Typography.small,
  },
  footer: {
    alignItems: "center",
    paddingTop: Spacing.xl,
    gap: Spacing.xs,
  },
  footerText: {
    ...Typography.body,
    fontWeight: "600",
  },
  footerSubtext: {
    ...Typography.small,
  },
});

// ============================================================
