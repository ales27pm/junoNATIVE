// ============================================================

import React from "react";
import { View, StyleSheet, ScrollView } from "react-native";
import { ThemedText } from "@/components/ThemedText";
import Knob from "@/components/Knob";
import SynthSwitch from "@/components/SynthSwitch";
import ChorusSelector from "@/components/ChorusSelector";
import { Colors, Spacing, Typography } from "@/constants/theme";
import { SynthParams, SynthSwitches } from "@/hooks/useSynthEngine";

interface ControlPanelProps {
  params: SynthParams;
  switches: SynthSwitches;
  onParamChange: (key: keyof SynthParams, value: number) => void;
  onSwitchChange: <K extends keyof SynthSwitches>(key: K, value: SynthSwitches[K]) => void;
}

export default function ControlPanel({
  params,
  switches,
  onParamChange,
  onSwitchChange,
}: ControlPanelProps) {
  const theme = Colors.dark;

  return (
    <ScrollView
      horizontal
      showsHorizontalScrollIndicator={false}
      style={styles.container}
      contentContainerStyle={styles.content}
    >
      <View style={styles.section}>
        <ThemedText style={[styles.sectionTitle, { color: theme.accent }]}>
          LFO
        </ThemedText>
        <View style={styles.sectionContent}>
          <Knob
            label="RATE"
            value={params.lfoRate}
            onChange={(v) => onParamChange("lfoRate", v)}
            defaultValue={50}
          />
          <Knob
            label="DELAY"
            value={params.lfoDelay}
            onChange={(v) => onParamChange("lfoDelay", v)}
            defaultValue={0}
          />
        </View>
      </View>

      <View style={[styles.divider, { backgroundColor: theme.divider }]} />

      <View style={styles.section}>
        <ThemedText style={[styles.sectionTitle, { color: theme.accent }]}>
          DCO
        </ThemedText>
        <View style={styles.sectionContent}>
          <Knob
            label="LFO MOD"
            value={params.dcoLfoMod}
            onChange={(v) => onParamChange("dcoLfoMod", v)}
            defaultValue={0}
          />
          <Knob
            label="PWM"
            value={params.dcoPwmDepth}
            onChange={(v) => onParamChange("dcoPwmDepth", v)}
            defaultValue={64}
          />
          <Knob
            label="NOISE"
            value={params.dcoNoiseLevel}
            onChange={(v) => onParamChange("dcoNoiseLevel", v)}
            defaultValue={0}
          />
          <Knob
            label="SUB"
            value={params.dcoSubLevel}
            onChange={(v) => onParamChange("dcoSubLevel", v)}
            defaultValue={64}
          />
        </View>
        <View style={styles.switchRow}>
          <SynthSwitch
            label="16'"
            value={switches.dcoFoot16}
            onChange={(v) => onSwitchChange("dcoFoot16", v)}
          />
          <SynthSwitch
            label="8'"
            value={switches.dcoFoot8}
            onChange={(v) => onSwitchChange("dcoFoot8", v)}
          />
          <SynthSwitch
            label="4'"
            value={switches.dcoFoot4}
            onChange={(v) => onSwitchChange("dcoFoot4", v)}
          />
          <SynthSwitch
            label="PULSE"
            value={switches.pulseWaveOn}
            onChange={(v) => onSwitchChange("pulseWaveOn", v)}
          />
          <SynthSwitch
            label="SAW"
            value={switches.sawWaveOn}
            onChange={(v) => onSwitchChange("sawWaveOn", v)}
          />
        </View>
      </View>

      <View style={[styles.divider, { backgroundColor: theme.divider }]} />

      <View style={styles.section}>
        <ThemedText style={[styles.sectionTitle, { color: theme.accent }]}>
          VCF
        </ThemedText>
        <View style={styles.sectionContent}>
          <Knob
            label="CUTOFF"
            value={params.vcfCutoff}
            onChange={(v) => onParamChange("vcfCutoff", v)}
            defaultValue={100}
            formatValue={(v) => `${Math.round((v / 127) * 20000)}Hz`}
          />
          <Knob
            label="RES"
            value={params.vcfResonance}
            onChange={(v) => onParamChange("vcfResonance", v)}
            defaultValue={30}
          />
          <Knob
            label="ENV"
            value={params.vcfEnvMod}
            onChange={(v) => onParamChange("vcfEnvMod", v)}
            defaultValue={50}
          />
          <Knob
            label="LFO"
            value={params.vcfLfoMod}
            onChange={(v) => onParamChange("vcfLfoMod", v)}
            defaultValue={0}
          />
          <Knob
            label="KEY"
            value={params.vcfKeyFollow}
            onChange={(v) => onParamChange("vcfKeyFollow", v)}
            defaultValue={64}
          />
        </View>
        <View style={styles.switchRow}>
          <SynthSwitch
            label="ENV +"
            value={switches.vcfEnvPositive}
            onChange={(v) => onSwitchChange("vcfEnvPositive", v)}
          />
        </View>
      </View>

      <View style={[styles.divider, { backgroundColor: theme.divider }]} />

      <View style={styles.section}>
        <ThemedText style={[styles.sectionTitle, { color: theme.accent }]}>
          VCA
        </ThemedText>
        <View style={styles.sectionContent}>
          <Knob
            label="LEVEL"
            value={params.vcaLevel}
            onChange={(v) => onParamChange("vcaLevel", v)}
            defaultValue={100}
          />
        </View>
        <View style={styles.switchRow}>
          <SynthSwitch
            label="ENV"
            value={switches.vcaModeEnv}
            onChange={(v) => onSwitchChange("vcaModeEnv", v)}
          />
        </View>
      </View>

      <View style={[styles.divider, { backgroundColor: theme.divider }]} />

      <View style={styles.section}>
        <ThemedText style={[styles.sectionTitle, { color: theme.accent }]}>
          ENV
        </ThemedText>
        <View style={styles.sectionContent}>
          <Knob
            label="A"
            value={params.envAttack}
            onChange={(v) => onParamChange("envAttack", v)}
            defaultValue={10}
          />
          <Knob
            label="D"
            value={params.envDecay}
            onChange={(v) => onParamChange("envDecay", v)}
            defaultValue={40}
          />
          <Knob
            label="S"
            value={params.envSustain}
            onChange={(v) => onParamChange("envSustain", v)}
            defaultValue={80}
          />
          <Knob
            label="R"
            value={params.envRelease}
            onChange={(v) => onParamChange("envRelease", v)}
            defaultValue={30}
          />
        </View>
      </View>

      <View style={[styles.divider, { backgroundColor: theme.divider }]} />

      <View style={styles.section}>
        <ThemedText style={[styles.sectionTitle, { color: theme.accent }]}>
          EFFECTS
        </ThemedText>
        <View style={styles.sectionContent}>
          <ChorusSelector
            value={switches.chorusMode}
            onChange={(v) => onSwitchChange("chorusMode", v)}
          />
        </View>
      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  content: {
    flexDirection: "row",
    paddingHorizontal: Spacing.lg,
    paddingVertical: Spacing.md,
    gap: Spacing.lg,
  },
  section: {
    alignItems: "center",
    gap: Spacing.sm,
  },
  sectionTitle: {
    ...Typography.sectionHeader,
    marginBottom: Spacing.xs,
  },
  sectionContent: {
    flexDirection: "row",
    gap: Spacing.md,
  },
  switchRow: {
    flexDirection: "row",
    gap: Spacing.md,
    marginTop: Spacing.sm,
  },
  divider: {
    width: 1,
    alignSelf: "stretch",
    marginVertical: Spacing.lg,
  },
});


// ============================================================
