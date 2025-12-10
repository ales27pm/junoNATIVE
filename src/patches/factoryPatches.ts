// ============================================================

export type UINamedPatch = {
  id: string;
  name: string;
  description?: string;
  bytes: number[]; // full SysEx message (including F0..F7)
};

export const FACTORY_PATCHES: UINamedPatch[] = [
  {
    id: 'juno-106-init',
    name: 'Init Patch',
    description: 'Neutral starting point for calibration',
    bytes: [
      0xF0, 0x41, 0x00, 0x29, 0x24,
      // ... real payload bytes go here ...
      0x00, // checksum
      0xF7,
    ],
  },
  {
    id: 'juno-106-brass',
    name: 'Juno Brass',
    description: 'Classic 80s brass approximation',
    bytes: [
      0xF0, 0x41, 0x00, 0x29, 0x24,
      // ... payload bytes ...
      0x00,
      0xF7,
    ],
  },
];


// ============================================================
