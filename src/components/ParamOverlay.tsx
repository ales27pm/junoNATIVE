import React from 'react';
import { Animated, StyleSheet, Text } from 'react-native';

export type ParamOverlayProps = {
  param?: string | number | null;
  value?: number | null;
  visible?: boolean;
};

export const ParamOverlay: React.FC<ParamOverlayProps> = ({
  param,
  value,
  visible = true,
}) => {
  if (!visible || param == null || value == null) {
    return null;
  }

  return (
    <Animated.View style={styles.box}>
      <Text style={styles.text}>
        {String(param).toUpperCase()} → {Number(value).toFixed(2)}
      </Text>
    </Animated.View>
  );
};

const styles = StyleSheet.create({
  box: {
    position: 'absolute',
    top: 30,
    alignSelf: 'center',
    backgroundColor: 'rgba(0,0,0,0.8)',
    padding: 8,
    borderRadius: 6,
  },
  text: {
    color: '#fff',
    fontSize: 12,
  },
});

export default ParamOverlay;
