// ============================================================

import React, { createContext, useContext, ReactNode } from "react";
import { useSynthEngine, SynthParams, SynthSwitches, Patch } from "@/hooks/useSynthEngine";

interface SynthContextType {
  params: SynthParams;
  switches: SynthSwitches;
  patches: Patch[];
  currentPatchId: string | null;
  activeNotes: Set<number>;
  isLoading: boolean;
  setParam: (key: keyof SynthParams, value: number) => void;
  setSwitch: <K extends keyof SynthSwitches>(key: K, value: SynthSwitches[K]) => void;
  noteOn: (note: number, velocity?: number) => void;
  noteOff: (note: number) => void;
  loadPatch: (patch: Patch) => Promise<void>;
  savePatch: (name: string, category: Patch["category"]) => Promise<Patch>;
  deletePatch: (patchId: string) => Promise<void>;
  getCurrentPatch: () => Patch | null;
}

const SynthContext = createContext<SynthContextType | null>(null);

export function SynthProvider({ children }: { children: ReactNode }) {
  const synthEngine = useSynthEngine();

  return (
    <SynthContext.Provider value={synthEngine}>{children}</SynthContext.Provider>
  );
}

export function useSynth() {
  const context = useContext(SynthContext);
  if (!context) {
    throw new Error("useSynth must be used within a SynthProvider");
  }
  return context;
}


// ============================================================
