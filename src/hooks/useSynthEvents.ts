import { useEffect } from 'react';
import { NativeEventEmitter, NativeModules, Platform } from 'react-native';

type SynthEventHandlers = {
  onParam?: (event: unknown) => void;
  onPatch?: (event: unknown) => void;
  onError?: (event: unknown) => void;
};

export default function useSynthEvents({ onParam, onPatch, onError }: SynthEventHandlers = {}) {
  useEffect(() => {
    const { RTNJunoEngine } = NativeModules as { RTNJunoEngine?: unknown };
    if (!RTNJunoEngine || Platform.OS !== 'ios') return undefined;

    const emitter = new NativeEventEmitter(RTNJunoEngine as object);
    const subs = [
      emitter.addListener('ParameterChanged', (e) => onParam?.(e)),
      emitter.addListener('PatchLoaded', (e) => onPatch?.(e)),
      emitter.addListener('EngineError', (e) => onError?.(e)),
    ];

    return () => subs.forEach((s) => s.remove());
  }, [onParam, onPatch, onError]);
}
