import { useEffect, useRef } from 'react';
import { NativeEventEmitter, NativeModules, Platform } from 'react-native';

type SynthEventHandlers = {
  onParam?: (event: unknown) => void;
  onPatch?: (event: unknown) => void;
  onError?: (event: unknown) => void;
};

export default function useSynthEvents({ onParam, onPatch, onError }: SynthEventHandlers = {}) {
  const handlersRef = useRef({ onParam, onPatch, onError });

  useEffect(() => {
    handlersRef.current = { onParam, onPatch, onError };
  }, [onParam, onPatch, onError]);

  useEffect(() => {
    const { RTNJunoEngine } = NativeModules as { RTNJunoEngine?: unknown };
    if (!RTNJunoEngine || Platform.OS !== 'ios') {
      return undefined;
    }

    const emitter = new NativeEventEmitter(RTNJunoEngine as object);
    const subs = [
      emitter.addListener('ParameterChanged', (e) => handlersRef.current.onParam?.(e)),
      emitter.addListener('PatchLoaded', (e) => handlersRef.current.onPatch?.(e)),
      emitter.addListener('EngineError', (e) => handlersRef.current.onError?.(e)),
    ];

    return () => subs.forEach((s) => s.remove());
  }, []);
}
