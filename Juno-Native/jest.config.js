module.exports = {
  preset: "react-native",
  transform: {
    "^.+\\.(js|ts|tsx)$": ["babel-jest", { configFile: "./babel.config.js" }],
  },
  moduleNameMapper: {
    "^@/(.*)$": "<rootDir>/client/$1",
    "^@shared/(.*)$": "<rootDir>/shared/$1",
  },
  testPathIgnorePatterns: ["/node_modules/", "/build/", "/android/", "/ios/"],
  cacheDirectory: ".jest-cache",
};
