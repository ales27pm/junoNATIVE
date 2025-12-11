#!/usr/bin/env node

const fs = require("fs");
const path = require("path");

const packageJsonPath = path.join(__dirname, "..", "package.json");

function fail(message) {
  console.error(`[validate-package-json] ${message}`);
  process.exit(1);
}

if (!fs.existsSync(packageJsonPath)) {
  fail(`package.json not found at ${packageJsonPath}`);
}

const raw = fs.readFileSync(packageJsonPath, "utf8");
let pkg;
try {
  pkg = JSON.parse(raw);
} catch (e) {
  fail(`Invalid JSON in package.json: ${e.message}`);
}

if (!pkg.name || pkg.name.trim().length === 0) {
  fail("package.json is missing a non-empty 'name' field.");
}

if (!pkg.version || pkg.version.trim().length === 0) {
  fail("package.json is missing a non-empty 'version' field.");
}

if (!pkg.scripts || typeof pkg.scripts !== "object") {
  fail("package.json is missing a 'scripts' section.");
}

if (!pkg.scripts["ios-ci"]) {
  fail("package.json is missing an 'ios-ci' script used by CI.");
}

console.log("[validate-package-json] package.json looks valid for iOS CI.");
process.exit(0);
