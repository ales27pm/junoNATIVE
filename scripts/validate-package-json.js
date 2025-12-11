const fs = require('fs');
const path = require('path');

const packageFiles = [
  path.join(__dirname, '..', 'package.json'),
];

let hasFailure = false;

function validatePackage(filePath) {
  const rel = path.relative(process.cwd(), filePath);
  try {
    const raw = fs.readFileSync(filePath, 'utf8');
    const json = JSON.parse(raw);

    if (!json.private) {
      console.warn(`[validate-package-json] ${rel}: "private": true is recommended for app repos.`);
    }

    if (!json.name || typeof json.name !== 'string') {
      console.error(`[validate-package-json] ${rel}: missing or invalid "name" field.`);
      hasFailure = true;
    }

    if (!json.version || typeof json.version !== 'string') {
      console.error(`[validate-package-json] ${rel}: missing or invalid "version" field.`);
      hasFailure = true;
    }
  } catch (err) {
    console.error(`[validate-package-json] Failed to parse ${rel}:`, err.message);
    hasFailure = true;
  }
}

for (const file of packageFiles) {
  if (fs.existsSync(file)) {
    validatePackage(file);
  } else {
    console.warn(`[validate-package-json] Skipping missing package file: ${file}`);
  }
}

if (hasFailure) {
  process.exitCode = 1;
}
