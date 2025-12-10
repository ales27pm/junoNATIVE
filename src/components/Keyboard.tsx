import React, { useMemo } from 'react';
import { Pressable, StyleSheet, View } from 'react-native';

const WHITE_NOTES = [0, 2, 4, 5, 7, 9, 11];
const KEY_W = 32;
const BLACK_W = 22;

export type KeyboardProps = {
  onNoteOn?: (note: number, velocity?: number) => void;
  onNoteOff?: (note: number) => void;
  activeNotes?: Set<number> | number[];
  startOctave?: number;
  numOctaves?: number;
  disabled?: boolean;
};

export const Keyboard: React.FC<KeyboardProps> = ({
  onNoteOn,
  onNoteOff,
  activeNotes = new Set<number>(),
  startOctave = 3,
  numOctaves = 2,
  disabled = false,
}) => {
  const notes = useMemo(() => {
    const arr: number[] = [];
    for (let o = 0; o < numOctaves; o += 1) {
      for (let n = 0; n < 12; n += 1) {
        arr.push(o * 12 + n + startOctave * 12);
      }
    }
    return arr;
  }, [startOctave, numOctaves]);

  const activeSet = useMemo<Set<number>>(() => {
    if (activeNotes instanceof Set) return activeNotes;
    return new Set(activeNotes ?? []);
  }, [activeNotes]);

  const isBlack = (n: number) => !WHITE_NOTES.includes(n % 12);

  return (
    <View style={styles.container}>
      {notes.map((n) => {
        const black = isBlack(n);
        const isActive = activeSet.has(n);
        return (
          <Pressable
            key={n}
            disabled={disabled}
            onPressIn={() => onNoteOn?.(n, 0.8)}
            onPressOut={() => onNoteOff?.(n)}
            style={[
              styles.key,
              black ? styles.black : styles.white,
              isActive && (black ? styles.blackA : styles.whiteA),
            ]}
          />
        );
      })}
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flexDirection: 'row',
    backgroundColor: '#111',
    paddingVertical: 10,
  },
  key: {
    borderWidth: 1,
    borderColor: '#333',
    marginHorizontal: 1,
  },
  white: {
    backgroundColor: '#fefefe',
    width: KEY_W,
    height: 140,
    zIndex: 1,
  },
  black: {
    backgroundColor: '#000',
    width: BLACK_W,
    height: 90,
    marginLeft: -BLACK_W / 2,
    marginRight: -BLACK_W / 2,
    zIndex: 2,
  },
  whiteA: {
    backgroundColor: '#ff6b6b',
  },
  blackA: {
    backgroundColor: '#ff4747',
  },
});

export default Keyboard;
