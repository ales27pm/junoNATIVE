const fs = require('fs');
const path = require('path');

const packageFiles = [
  path.join(__dirname, '..', 'package.json'),
  path.join(__dirname, '..', 'Juno-Native', 'package.json'),
];

const hasComment = (line) => /^\s*\/\//.test(line);

let hasFailure = false;

for (const filePath of packageFiles) {
  const raw = fs.readFileSync(filePath, 'utf8');
  const lines = raw.split(/\r?\n/);
  const commentLines = lines
    .map((line, index) => ({ line, index: index + 1 }))
    .filter(({ line }) => hasComment(line));

  if (commentLines.length > 0) {
    console.error(
      `Found line comments in ${path.relative(process.cwd(), filePath)} at lines: ${commentLines
        .map(({ index }) => index)
        .join(', ')}. Remove '//'-prefixed separators to keep JSON valid.`,
    );
    hasFailure = true;
    continue;
  }

  try {
    JSON.parse(raw);
    console.log(`✔️  ${path.relative(process.cwd(), filePath)} is valid JSON.`);
  } catch (error) {
    console.error(`❌ ${path.relative(process.cwd(), filePath)} is not valid JSON: ${error.message}`);
    hasFailure = true;
  }
}

if (hasFailure) {
  process.exitCode = 1;
}
