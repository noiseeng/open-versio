/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { DEFAULT_CONFIG, firmwareName, loadBuildConfig } from "./buildArgs.ts";
import { REPO_ROOT } from "./tools.ts";
import { findImage, flashImage, listDevices, loadUsb } from "./usbFlash.ts";

async function main(): Promise<void> {
  const args = process.argv.slice(2);
  const host = await loadUsb();
  if (args.includes("--list")) {
    await listDevices(host);
    return;
  }
  const firmwares = loadBuildConfig(REPO_ROOT).firmwares.map((f) => f.name);
  const name = args.find((a) => !a.startsWith("-"));
  await flashImage(host, findImage(DEFAULT_CONFIG, firmwareName(firmwares, name)));
}

main().then(
  () => process.exit(0),
  (e) => {
    process.stderr.write(`flash: ${(e as Error).message} (cwd ${process.cwd()})\n`);
    process.exit(1);
  },
);
