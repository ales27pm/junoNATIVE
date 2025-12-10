import React, { useEffect, useMemo, useRef, useState } from 'react';
import { PanResponder, StyleSheet, Text, View } from 'react-native';

export type KnobProps = {
  label?: string;
  value?: number;
  externalValue?: number;
  onChange?: (value: number) => void;
};

export const Knob: React.FC<KnobProps> = ({
  label,
  value = 0,
  externalValue,
  onChange,
}) => {
  const [val, setVal] = useState<number>(value);
  const valueRef = useRef(val);

  useEffect(() => {
    valueRef.current = val;
  }, [val]);

  useEffect(() => {
    if (externalValue !== undefined && externalValue !== valueRef.current) {
      setVal(externalValue);
    }
  }, [externalValue]);

  const clamp = (v: number) => Math.min(1, Math.max(0, v));

  const pan = useMemo(
    () =>
      PanResponder.create({
        onStartShouldSetPanResponder: () => true,
        onPanResponderMove: (_, gestureState) => {
          setVal((prev) => {
            const next = clamp(prev - gestureState.dy / 150);
            valueRef.current = next;
            onChange?.(next);
            return next;
          });
        },
      }),
    [onChange],
  );

  return (
    <View style={styles.wrap}>
      <View {...pan.panHandlers} style={styles.knob}>
        <View
          style={[
            styles.indicator,
            { transform: [{ rotate: `${val * 270 - 135}deg` }] },
          ]}
        />
      </View>
      {label ? <Text style={styles.label}>{label}</Text> : null}
    </View>
  );
};

const styles = StyleSheet.create({
  wrap: {
    alignItems: 'center',
    margin: 8,
  },
  knob: {
    width: 60,
    height: 60,
    borderRadius: 30,
    backgroundColor: '#333',
    justifyContent: 'center',
    alignItems: 'center',
  },
  indicator: {
    width: 4,
    height: 20,
    backgroundColor: '#ff6b6b',
    borderRadius: 2,
  },
  label: {
    color: '#ccc',
    marginTop: 4,
  },
});

export default Knob;
