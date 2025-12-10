import { clamp } from "../shared/utils/math";

describe("clamp", () => {
  it("returns value when within bounds", () => {
    expect(clamp(5, 0, 10)).toBe(5);
  });

  it("clamps to minimum", () => {
    expect(clamp(-2, 0, 10)).toBe(0);
  });

  it("clamps to maximum", () => {
    expect(clamp(22, -5, 10)).toBe(10);
  });

  it("throws on invalid bounds", () => {
    expect(() => clamp(1, 5, 0)).toThrow(RangeError);
  });

  it("throws on non-number arguments", () => {
    // @ts-expect-error intentional invalid input for runtime guard
    expect(() => clamp(NaN, 0, 1)).toThrow(TypeError);
  });
});
