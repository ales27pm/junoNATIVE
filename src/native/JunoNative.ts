// ============================================================

import { NativeModules, NativeEventEmitter } from 'react-native';

type JunoNativeModuleType = {
  loadJuno106Sysex(bytes: number[], name: string): Promise<boolean>;
  setJuno106Mapping(mapping: { [paramName: string]: number }): void;
};

const { NativeJunoEngine } = NativeModules as {
  NativeJunoEngine: JunoNativeModuleType;
};

export const JunoNativeEngine = NativeJunoEngine;

const emitter = new NativeEventEmitter(NativeJunoEngine);

export function onPatchLoaded(
  handler: (info: { name: string; size: number }) => void,
) {
  const sub = emitter.addListener('JunoPatchLoaded', handler);
  return () => sub.remove();
}


// ============================================================
