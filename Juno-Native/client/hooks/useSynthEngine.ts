// ============================================================

import { useState, useCallback, useRef, useEffect } from "react";
import { Platform } from "react-native";
import AsyncStorage from "@react-native-async-storage/async-storage";
import { webAudioSynth } from "@/lib/WebAudioSynth";

export interface SynthParams {
  lfoRate: number;
  lfoDelay: number;
  dcoLfoMod: number;
  dcoPwmDepth: number;
  dcoNoiseLevel: number;
  dcoSubLevel: number;
  vcfCutoff: number;
  vcfResonance: number;
  vcfEnvMod: number;
  vcfLfoMod: number;
  vcfKeyFollow: number;
  vcaLevel: number;
  envAttack: number;
  envDecay: number;
  envSustain: number;
  envRelease: number;
}

export interface SynthSwitches {
  dcoFoot16: boolean;
  dcoFoot8: boolean;
  dcoFoot4: boolean;
  pulseWaveOn: boolean;
  sawWaveOn: boolean;
  chorusMode: 0 | 1 | 2;
  pwmSourceLFO: boolean;
  vcfEnvPositive: boolean;
  vcaModeEnv: boolean;
  hpfSetting: 0 | 1 | 2 | 3;
}

export interface Patch {
  id: string;
  name: string;
  category: "bass" | "lead" | "pad" | "fx";
  params: SynthParams;
  switches: SynthSwitches;
  createdAt: number;
}

const defaultParams: SynthParams = {
  lfoRate: 50,
  lfoDelay: 0,
  dcoLfoMod: 0,
  dcoPwmDepth: 64,
  dcoNoiseLevel: 0,
  dcoSubLevel: 64,
  vcfCutoff: 100,
  vcfResonance: 30,
  vcfEnvMod: 50,
  vcfLfoMod: 0,
  vcfKeyFollow: 64,
  vcaLevel: 100,
  envAttack: 10,
  envDecay: 40,
  envSustain: 80,
  envRelease: 30,
};

const defaultSwitches: SynthSwitches = {
  dcoFoot16: false,
  dcoFoot8: true,
  dcoFoot4: false,
  pulseWaveOn: true,
  sawWaveOn: true,
  chorusMode: 1,
  pwmSourceLFO: true,
  vcfEnvPositive: true,
  vcaModeEnv: true,
  hpfSetting: 0,
};

const PATCHES_STORAGE_KEY = "@juno106_patches";
const CURRENT_PATCH_KEY = "@juno106_current_patch";

export function useSynthEngine() {
  const [params, setParams] = useState<SynthParams>(defaultParams);
  const [switches, setSwitches] = useState<SynthSwitches>(defaultSwitches);
  const [patches, setPatches] = useState<Patch[]>([]);
  const [currentPatchId, setCurrentPatchId] = useState<string | null>(null);
  const [activeNotes, setActiveNotes] = useState<Set<number>>(new Set());
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    loadPatches();
  }, []);

  useEffect(() => {
    if (Platform.OS === "web") {
      webAudioSynth.updateParams(params, switches);
    }
  }, [params, switches]);

  const loadPatches = async () => {
    try {
      const stored = await AsyncStorage.getItem(PATCHES_STORAGE_KEY);
      if (stored) {
        const parsed = JSON.parse(stored) as Patch[];
        setPatches(parsed);
      } else {
        const factory = getFactoryPatches();
        setPatches(factory);
        await AsyncStorage.setItem(PATCHES_STORAGE_KEY, JSON.stringify(factory));
      }
      
      const currentId = await AsyncStorage.getItem(CURRENT_PATCH_KEY);
      if (currentId) {
        setCurrentPatchId(currentId);
      }
    } catch (error) {
      console.error("Failed to load patches:", error);
    } finally {
      setIsLoading(false);
    }
  };

  const savePatches = async (newPatches: Patch[]) => {
    try {
      await AsyncStorage.setItem(PATCHES_STORAGE_KEY, JSON.stringify(newPatches));
      setPatches(newPatches);
    } catch (error) {
      console.error("Failed to save patches:", error);
    }
  };

  const setParam = useCallback((key: keyof SynthParams, value: number) => {
    setParams((prev) => ({ ...prev, [key]: Math.max(0, Math.min(127, value)) }));
  }, []);

  const setSwitch = useCallback(<K extends keyof SynthSwitches>(
    key: K,
    value: SynthSwitches[K]
  ) => {
    setSwitches((prev) => ({ ...prev, [key]: value }));
  }, []);

  const noteOn = useCallback((note: number, velocity: number = 100) => {
    setActiveNotes((prev) => new Set(prev).add(note));
    if (Platform.OS === "web") {
      webAudioSynth.noteOn(note, velocity);
    }
  }, []);

  const noteOff = useCallback((note: number) => {
    setActiveNotes((prev) => {
      const next = new Set(prev);
      next.delete(note);
      return next;
    });
    if (Platform.OS === "web") {
      webAudioSynth.noteOff(note);
    }
  }, []);

  const loadPatch = useCallback(async (patch: Patch) => {
    setParams(patch.params);
    setSwitches(patch.switches);
    setCurrentPatchId(patch.id);
    await AsyncStorage.setItem(CURRENT_PATCH_KEY, patch.id);
  }, []);

  const savePatch = useCallback(async (name: string, category: Patch["category"]) => {
    const newPatch: Patch = {
      id: Date.now().toString(),
      name,
      category,
      params: { ...params },
      switches: { ...switches },
      createdAt: Date.now(),
    };
    const newPatches = [...patches, newPatch];
    await savePatches(newPatches);
    setCurrentPatchId(newPatch.id);
    await AsyncStorage.setItem(CURRENT_PATCH_KEY, newPatch.id);
    return newPatch;
  }, [params, switches, patches]);

  const deletePatch = useCallback(async (patchId: string) => {
    const newPatches = patches.filter((p) => p.id !== patchId);
    await savePatches(newPatches);
    if (currentPatchId === patchId) {
      setCurrentPatchId(null);
      await AsyncStorage.removeItem(CURRENT_PATCH_KEY);
    }
  }, [patches, currentPatchId]);

  const getCurrentPatch = useCallback(() => {
    return patches.find((p) => p.id === currentPatchId) || null;
  }, [patches, currentPatchId]);

  return {
    params,
    switches,
    patches,
    currentPatchId,
    activeNotes,
    isLoading,
    setParam,
    setSwitch,
    noteOn,
    noteOff,
    loadPatch,
    savePatch,
    deletePatch,
    getCurrentPatch,
  };
}

function getFactoryPatches(): Patch[] {
  return [
    {
      id: "factory_1",
      name: "Classic Pad",
      category: "pad",
      params: {
        ...defaultParams,
        vcfCutoff: 80,
        vcfResonance: 20,
        envAttack: 60,
        envDecay: 80,
        envSustain: 90,
        envRelease: 70,
      },
      switches: { ...defaultSwitches, chorusMode: 2 },
      createdAt: 0,
    },
    {
      id: "factory_2",
      name: "Fat Bass",
      category: "bass",
      params: {
        ...defaultParams,
        vcfCutoff: 60,
        vcfResonance: 50,
        dcoSubLevel: 100,
        envAttack: 0,
        envDecay: 40,
        envSustain: 60,
        envRelease: 20,
      },
      switches: { ...defaultSwitches, dcoFoot16: true, sawWaveOn: true, pulseWaveOn: false },
      createdAt: 0,
    },
    {
      id: "factory_3",
      name: "Bright Lead",
      category: "lead",
      params: {
        ...defaultParams,
        vcfCutoff: 110,
        vcfResonance: 60,
        envAttack: 0,
        envDecay: 30,
        envSustain: 70,
        envRelease: 25,
        vcfEnvMod: 70,
      },
      switches: { ...defaultSwitches, sawWaveOn: true, pulseWaveOn: false, chorusMode: 1 },
      createdAt: 0,
    },
    {
      id: "factory_4",
      name: "Resonant Sweep",
      category: "fx",
      params: {
        ...defaultParams,
        vcfCutoff: 40,
        vcfResonance: 90,
        dcoLfoMod: 30,
        lfoRate: 20,
        vcfLfoMod: 50,
        envAttack: 80,
        envDecay: 100,
        envSustain: 50,
        envRelease: 80,
      },
      switches: { ...defaultSwitches, chorusMode: 2 },
      createdAt: 0,
    },
    {
      id: "factory_5",
      name: "PWM Strings",
      category: "pad",
      params: {
        ...defaultParams,
        dcoPwmDepth: 80,
        dcoLfoMod: 40,
        lfoRate: 35,
        vcfCutoff: 90,
        envAttack: 50,
        envDecay: 60,
        envSustain: 85,
        envRelease: 60,
      },
      switches: { ...defaultSwitches, pulseWaveOn: true, sawWaveOn: false, pwmSourceLFO: true, chorusMode: 2 },
      createdAt: 0,
    },
    {
      id: "factory_6",
      name: "Pluck",
      category: "lead",
      params: {
        ...defaultParams,
        vcfCutoff: 100,
        vcfResonance: 30,
        vcfEnvMod: 80,
        envAttack: 0,
        envDecay: 25,
        envSustain: 0,
        envRelease: 20,
      },
      switches: { ...defaultSwitches, sawWaveOn: true, pulseWaveOn: true },
      createdAt: 0,
    },
  ];
}

export { defaultParams, defaultSwitches };


// ============================================================
