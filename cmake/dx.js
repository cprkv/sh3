const path = require('path');
const fs = require('fs');

const WINDOWS_SDK_DIR = process.argv[2];
const WINDOWS_SDK_VERSION = process.argv[3];
const TARGET_ARCHITECTURE = process.argv[4];
const REQUIRED_LIBS = process.argv.slice(5);

// console.error("WINDOWS_SDK_DIR:", WINDOWS_SDK_DIR);
// console.error("WINDOWS_SDK_VERSION:", WINDOWS_SDK_VERSION);
// console.error("TARGET_ARCHITECTURE:", TARGET_ARCHITECTURE);
// console.error("REQUIRED_LIBS:", REQUIRED_LIBS);

const dirEntriesStartsWith = (p, startsWith) => fs.readdirSync(p)
  .filter(x => x.startsWith(startsWith))
  .map(x => path.join(p, x));

const exists = (...args) => fs.existsSync(path.join(...args));

function checkPath(what, prev, ...args) {
  if (prev) {
    return prev;
  }
  if (!exists(...args, what)) {
    // console.error(`directory ${path.join(...args)} not contains "${what}"`)
    return null;
  }
  return path.join(...args);
}

const checkInclude = (prev, ...args) => checkPath("d3d11.h", prev, ...args);
const checkLib = (prev, ...args) => checkPath("d3d11.lib", prev, ...args);

function findDXSDK() {
  let dxSDKSearchPaths = [];

  if (process.env.ProgramFiles && exists(process.env.ProgramFiles))
    dxSDKSearchPaths.push(...dirEntriesStartsWith(process.env.ProgramFiles, "Microsoft DirectX SDK"));

  if (process.env['ProgramFiles(x86)'] && exists(process.env['ProgramFiles(x86)']))
    dxSDKSearchPaths.push(...dirEntriesStartsWith(process.env['ProgramFiles(x86)'], "Microsoft DirectX SDK"));

  if (process.env.DXSDK_DIR && exists(process.env.DXSDK_DIR))
    dxSDKSearchPaths.push(process.env.DXSDK_DIR);

  // console.error("dxSDKSearchPaths:", dxSDKSearchPaths);

  let include;
  let libs;

  for (const p of dxSDKSearchPaths) {
    include = checkInclude(include, p, "include");
    include = checkInclude(include, p, "include", TARGET_ARCHITECTURE);
    include = checkInclude(include, p, "Headers");
    libs = checkLib(libs, p, "lib");
    libs = checkLib(libs, p, "lib", TARGET_ARCHITECTURE);
    libs = checkLib(libs, p, "Libs");
  }

  return {libs, include, name: "directx sdk"};
}

function findWINSDK() {
  const dxLibPath = path.join(WINDOWS_SDK_DIR, "Lib", WINDOWS_SDK_VERSION, "um", TARGET_ARCHITECTURE);
  const dxIncPath = path.join(WINDOWS_SDK_DIR, "Include", WINDOWS_SDK_VERSION, "um");
  return {
    libs: checkLib(null, dxLibPath),
    include: checkInclude(null, dxIncPath),
    name: "windows sdk",
  };
}

for (const {libs, include, name} of [findWINSDK(), findDXSDK()]) {
  if (!libs || !include) {
    console.error(`can't use ${name} because it was not found`);
    continue;
  }

  if (!REQUIRED_LIBS.every(
    lib => exists(libs, `${lib}.lib`) || console.error(`can't use ${name}, no lib '${lib}' in ${libs}`)
  ))
    continue;

  process.stdout.write(`${libs};${include}`)
  return;
}

console.error("no suitable sdk for required libs found");
process.exit(1);