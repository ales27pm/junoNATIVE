// ============================================================

import React, { useEffect, useState } from 'react';
import { SafeAreaView, View, Text, StyleSheet, TouchableOpacity, ScrollView } from 'react-native';
import { JunoNative } from './src/native/JunoModule';
import { AnalogCharacterPanel } from './src/components/AnalogCharacterPanel';
import { PatchBrowser } from './src/components/PatchBrowser';
import type { AnalogSettings } from './src/native/JunoModule';

const defaultSettings: AnalogSettings = {
  dcoBeating: 0.7,
  filterDrift: 0.8,
  envelopeClick: 0.6,
  cableLength: 3.0,
  temperature: 0.5,
  age: 0.3,
};

const App = () => {
  const [analogEnabled, setAnalogEnabled] = useState(true);
  const [settings, setSettings] = useState<AnalogSettings>(defaultSettings);

  useEffect(() => {
    JunoNative.startAudio();
    JunoNative.setAnalogCharacter(defaultSettings);

    return () => {
      JunoNative.stopAudio();
    };
  }, []);

  useEffect(() => {
    if (analogEnabled) {
      JunoNative.setAnalogCharacter(settings);
    } else {
      JunoNative.setAnalogCharacter({
        ...settings,
        dcoBeating: 0,
        filterDrift: 0,
        envelopeClick: 0,
        cableLength: 0,
        temperature: 0,
        age: 0,
      });
    }
  }, [analogEnabled, settings]);

  const playChord = () => {
    const notes = [60, 64, 67];
    notes.forEach((n, idx) => {
      setTimeout(() => JunoNative.noteOn(n, 0.9), idx * 20);
    });
    setTimeout(() => {
      notes.forEach((n) => JunoNative.noteOff(n));
    }, 1000);
  };

  return (
    <SafeAreaView style={styles.root}>
      <ScrollView contentInsetAdjustmentBehavior="automatic">
        <View style={styles.header}>
          <Text style={styles.title}>JUNO-106 Pocket</Text>
          <Text style={styles.subtitle}>Circuit-level Juno emulation on iOS</Text>
        </View>

        <View style={styles.controls}>
          <TouchableOpacity style={styles.playButton} onPress={playChord}>
            <Text style={styles.playText}>Play C Major</Text>
          </TouchableOpacity>
        </View>

        <AnalogCharacterPanel
          enabled={analogEnabled}
          settings={settings}
          onChangeEnabled={setAnalogEnabled}
          onChangeSettings={setSettings}
        />

        <PatchBrowser />
      </ScrollView>
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  root: { flex: 1, backgroundColor: '#020617' },
  header: { padding: 16 },
  title: { color: '#e5e7eb', fontSize: 24, fontWeight: 'bold' },
  subtitle: { color: '#9ca3af', fontSize: 14, marginTop: 4 },
  controls: { paddingHorizontal: 16, paddingBottom: 8 },
  playButton: {
    backgroundColor: '#22c55e',
    paddingVertical: 12,
    borderRadius: 999,
    alignItems: 'center',
  },
  playText: { color: '#022c22', fontWeight: 'bold' },
});

export default App;


// ============================================================
