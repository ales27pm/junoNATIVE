// ============================================================

import React from 'react';
import { View, Text, Switch, StyleSheet } from 'react-native';
import Slider from '@react-native-community/slider';
import type { AnalogSettings } from '../native/JunoModule';

type Props = {
  enabled: boolean;
  settings: AnalogSettings;
  onChangeEnabled: (enabled: boolean) => void;
  onChangeSettings: (settings: AnalogSettings) => void;
};

export const AnalogCharacterPanel: React.FC<Props> = ({
  enabled,
  settings,
  onChangeEnabled,
  onChangeSettings,
}) => {
  const update = (key: keyof AnalogSettings, value: number) => {
    onChangeSettings({ ...settings, [key]: value });
  };

  return (
    <View style={styles.container}>
      <View style={styles.header}>
        <Text style={styles.title}>Analog Character</Text>
        <Switch value={enabled} onValueChange={onChangeEnabled} />
      </View>

      {enabled && (
        <>
          {renderSlider('DCO Beating', 'dcoBeating', settings.dcoBeating, update)}
          {renderSlider('Filter Drift', 'filterDrift', settings.filterDrift, update)}
          {renderSlider('Envelope Click', 'envelopeClick', settings.envelopeClick, update)}
          {renderSlider('Cable Length (m)', 'cableLength', settings.cableLength, update, 0, 10)}
          {renderSlider('Temperature', 'temperature', settings.temperature, update)}
          {renderSlider('Age', 'age', settings.age, update)}
        </>
      )}
    </View>
  );
};

function renderSlider(
  label: string,
  key: keyof AnalogSettings,
  value: number,
  update: (key: keyof AnalogSettings, v: number) => void,
  min: number = 0,
  max: number = 1,
) {
  return (
    <View style={styles.sliderRow} key={key}>
      <Text style={styles.label}>{label}</Text>
      <Slider
        style={styles.slider}
        minimumValue={min}
        maximumValue={max}
        value={value}
        onValueChange={(v) => update(key, v)}
      />
      <Text style={styles.value}>{value.toFixed(2)}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    backgroundColor: '#111827',
    padding: 16,
    borderRadius: 12,
    margin: 12,
  },
  header: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  title: {
    color: '#f9fafb',
    fontSize: 18,
    fontWeight: 'bold',
  },
  sliderRow: {
    flexDirection: 'row',
    alignItems: 'center',
    marginTop: 12,
  },
  label: {
    color: '#e5e7eb',
    width: 140,
    fontSize: 14,
  },
  slider: {
    flex: 1,
    marginHorizontal: 8,
  },
  value: {
    width: 50,
    textAlign: 'right',
    color: '#9ca3af',
  },
});


// ============================================================
