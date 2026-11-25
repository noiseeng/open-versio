/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { existsSync } from "node:fs";
import { join } from "node:path";
import { elfPath, loadBuildConfig } from "./buildArgs.ts";
import type { FirmwareConfig } from "./buildArgs.ts";
import { exec } from "./exec.ts";
import { faultLog, toolsAddr2line } from "./faultLog.ts";
import { flashJLink, jlinkDir, rttJLink } from "./jlink.ts";
import { parseRunCli } from "./jlinkArgs.ts";
import type { RunCli } from "./jlinkArgs.ts";
import { flashOpenOcd, rttOpenOcd } from "./openocd.ts";
import { REPO_ROOT } from "./tools.ts";
import { streamLog, utf8Writer } from "./usbLog.ts";
import { findImage, flashImage, loadUsb } from "./usbFlash.ts";

function elfFile(cli: RunCli): string {
  const firmware: FirmwareConfig = { name: cli.name, sources: [] };
  const elf = join(REPO_ROOT, elfPath(cli.config, firmware));
  if (!existsSync(elf)) throw new Error(`${elf} not found (cwd ${process.cwd()}); run without --no-build`);
  return elf;
}

async function build(cli: RunCli): Promise<void> {
  const script = join(REPO_ROOT, "scripts", "build.ts");
  const r = await exec(process.execPath, [...process.execArgv, script, "--config", cli.config, cli.name], true);
  if (r.code !== 0) throw new Error(`building ${cli.name} ${cli.config} failed with ${r.code}`);
}

const USB_HINT = "the module is muted with magenta LEDs and still running; fix the code and run npm run debug again";

const PROBE_HINT =
  "the core is halted in the fault handler; see the crash skill in .claude/skills to read it with the probe";

function log(elf: string, hint: string): (text: string) => void {
  return faultLog({
    elf,
    root: REPO_ROOT,
    hint,
    write: (text) => process.stdout.write(text),
    symbolize: toolsAddr2line(),
    settleMs: 500,
  });
}

async function main(): Promise<void> {
  const config = loadBuildConfig(REPO_ROOT);
  const cli = parseRunCli(
    process.argv.slice(2),
    config.firmwares.map((f) => f.name),
  );
  if (cli.probe === "usb") {
    process.stderr.write(`\n${cli.name} ${cli.config} over USB\n`);
    if (cli.build) await build(cli);
    const host = await loadUsb();
    await flashImage(host, findImage(cli.config, cli.name));
    if (cli.rtt) await streamLog(host, { write: utf8Writer(log(elfFile(cli), USB_HINT)) });
    return;
  }
  if (cli.probe === "stlink") {
    process.stderr.write(`\n${cli.name} ${cli.config} on ${config.openocdTarget} (ST-Link via OpenOCD)\n`);
    if (cli.build) await build(cli);
    const elf = elfFile(cli);
    await flashOpenOcd(config.openocdTarget, elf);
    if (cli.rtt) await rttOpenOcd(config.openocdTarget, elf, log(elf, PROBE_HINT));
    return;
  }
  const dir = jlinkDir();
  process.stderr.write(`\n${cli.name} ${cli.config} on ${config.jlinkDevice} (J-Link ${dir})\n`);
  if (cli.build) await build(cli);
  const elf = elfFile(cli);
  await flashJLink(dir, config.jlinkDevice, elf);
  if (cli.rtt) await rttJLink(dir, config.jlinkDevice, log(elf, PROBE_HINT));
}

main().then(
  () => process.exit(0),
  (e: unknown) => {
    process.stderr.write(`run: ${e instanceof Error ? e.message : String(e)}\n`);
    process.exit(1);
  },
);
