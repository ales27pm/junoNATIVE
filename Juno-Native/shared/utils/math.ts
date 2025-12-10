export function clamp(value: number, min: number, max: number): number {
  if (Number.isNaN(value) || Number.isNaN(min) || Number.isNaN(max)) {
    throw new TypeError("clamp expects numeric arguments");
  }

  if (min > max) {
    throw new RangeError("Minimum value cannot exceed maximum value");
  }

  if (value < min) return min;
  if (value > max) return max;
  return value;
}
