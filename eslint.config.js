const { FlatCompat } = require('@eslint/eslintrc');

const compat = new FlatCompat({
  baseDirectory: __dirname,
});

module.exports = [
  ...compat.extends('@react-native'),
  {
    ignores: [
      'node_modules/**',
      'build/**',
      'fastlane/build/**',
      'android/app/build/**',
      'ios/build/**',
    ],
  },
];
