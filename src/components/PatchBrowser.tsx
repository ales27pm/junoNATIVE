// ============================================================

import React, { useEffect, useState } from 'react';
import {
  View,
  Text,
  FlatList,
  TouchableOpacity,
  StyleSheet,
  Alert,
} from 'react-native';
import { FACTORY_PATCHES, UINamedPatch } from '../patches/factoryPatches';
import { JunoNativeEngine, onPatchLoaded } from '../native/JunoNative';

const DEFAULT_MAPPING: { [key: string]: number } = {
  vcfCutoff: 20,
  vcfResonance: 21,
  envAttack: 30,
  envDecay: 31,
  envSustain: 32,
  envRelease: 33,
  lfoRate: 10,
  lfoDelay: 11,
  dcoPulseWidth: 40,
  dcoLfoDepth: 41,
};

export const PatchBrowser: React.FC = () => {
  const [loadingId, setLoadingId] = useState<string | null>(null);

  useEffect(() => {
    JunoNativeEngine.setJuno106Mapping(DEFAULT_MAPPING);

    const unsub = onPatchLoaded(info => {
      setLoadingId(null);
      Alert.alert('Patch Loaded', `Loaded "${info.name}" (${info.size} bytes)`);
    });

    return unsub;
  }, []);

  const loadPatch = async (patch: UINamedPatch) => {
    try {
      setLoadingId(patch.id);
      const ok = await JunoNativeEngine.loadJuno106Sysex(patch.bytes, patch.name);
      if (!ok) {
        setLoadingId(null);
        Alert.alert('Error', 'SysEx not recognized as Roland/Juno');
      }
    } catch (e: any) {
      setLoadingId(null);
      Alert.alert('Error', e?.message ?? 'Failed to load patch');
    }
  };

  const renderItem = ({ item }: { item: UINamedPatch }) => {
    const isLoading = loadingId === item.id;
    return (
      <TouchableOpacity
        style={styles.patchRow}
        onPress={() => loadPatch(item)}
        disabled={isLoading}
      >
        <View style={styles.patchText}>
          <Text style={styles.patchName}>{item.name}</Text>
          {item.description ? (
            <Text style={styles.patchDesc}>{item.description}</Text>
          ) : null}
        </View>
        <View style={styles.patchMeta}>
          <Text style={styles.patchSize}>{item.bytes.length} bytes</Text>
          <Text style={[styles.loadLabel, isLoading && styles.loadLabelBusy]}>
            {isLoading ? 'Loading…' : 'Load'}
          </Text>
        </View>
      </TouchableOpacity>
    );
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Juno-106 Patch Librarian</Text>
      <Text style={styles.subtitle}>
        Tap a patch to send its SysEx to the internal Juno engine.
      </Text>
      <FlatList
        data={FACTORY_PATCHES}
        keyExtractor={p => p.id}
        renderItem={renderItem}
        ItemSeparatorComponent={() => <View style={styles.sep} />}
      />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    backgroundColor: '#111827',
    padding: 12,
    borderRadius: 10,
    marginVertical: 8,
  },
  title: {
    color: 'white',
    fontSize: 18,
    fontWeight: '600',
    marginBottom: 4,
  },
  subtitle: {
    color: '#9CA3AF',
    fontSize: 12,
    marginBottom: 8,
  },
  patchRow: {
    flexDirection: 'row',
    alignItems: 'center',
    paddingVertical: 10,
  },
  patchText: {
    flex: 1,
  },
  patchName: {
    color: 'white',
    fontSize: 14,
    fontWeight: '500',
  },
  patchDesc: {
    color: '#9CA3AF',
    fontSize: 11,
  },
  patchMeta: {
    alignItems: 'flex-end',
  },
  patchSize: {
    color: '#6B7280',
    fontSize: 10,
  },
  loadLabel: {
    marginTop: 4,
    color: '#38BDF8',
    fontSize: 12,
  },
  loadLabelBusy: {
    color: '#F59E0B',
  },
  sep: {
    height: 1,
    backgroundColor: '#1F2933',
  },
});


// ============================================================
