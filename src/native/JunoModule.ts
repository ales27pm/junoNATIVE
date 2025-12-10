// ============================================================

import { NativeModules } from 'react-native';

export type AnalogSettings = {
  dcoBeating: number;
  filterDrift: number;
  envelopeClick: number;
  cableLength: number;
  temperature: number;
  age: number;
};

type JunoModuleType = {
  startAudio(): void;
  stopAudio(): void;
  noteOn(midiNote: number, velocity: number): void;
  noteOff(midiNote: number): void;
  setPolyAftertouch(voiceIndex: number, pressure: number): void;
  setAnalogCharacter(settings: AnalogSettings): void;
};

const { JunoModule } = NativeModules as { JunoModule: JunoModuleType };

export const JunoNative = JunoModule;


// ============================================================
