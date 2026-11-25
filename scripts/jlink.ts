/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { closeSync, existsSync, openSync, readdirSync, readSync, statSync, writeFileSync } from "node:fs";
import { join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { exec } from "./exec.ts";
import {
  downloadConfirmed,
  findJLinkDir,
  flashArgs,
  flashCommands,
  jlinkTool,
  jlinkVersion,
  notFoundMessage,
  rttArgs,
} from "./jlinkArgs.ts";
import type { JLinkEnv } from "./jlinkArgs.ts";
import { scratchDir } from "./tools.ts";

export function hostJLinkEnv(): JLinkEnv {
  return {
    platform: process.platform,
    listDir: (dir) => (existsSync(dir) ? readdirSync(dir) : undefined),
    exists: existsSync,
  };
}

export function jlinkDir(): string {
  const dir = findJLinkDir(hostJLinkEnv());
  if (dir === undefined) throw new Error(notFoundMessage(hostJLinkEnv()));
  return dir;
}

export async function flashJLink(dir: string, device: string, elf: string): Promise<void> {
  const script = join(scratchDir("lacrun-"), "flash.jlink");
  writeFileSync(script, flashCommands(resolve(elf)));
  const r = await exec(jlinkTool(dir, "JLink", process.platform), flashArgs(device, script), false);
  process.stdout.write(r.output);
  if (r.code !== 0) throw new Error(`J-Link exited ${r.code}; is the probe attached and no debug session holding it?`);
  if (!downloadConfirmed(r.output))
    throw new Error("J-Link ran but never confirmed the download; see its output above");
}

export async function rttJLink(dir: string, device: string, out: (text: string) => void): Promise<void> {
  const log = join(scratchDir("lacrtt-"), "rtt.log");
  process.stderr.write(`\nRTT -> ${log} (ctrl-c to stop)\n`);
  let at = 0;
  let seen = 0;
  const pump = setInterval(() => {
    if (!existsSync(log)) return;
    const size = statSync(log).size;
    if (size <= at) return;
    const fd = openSync(log, "r");
    const buf = Buffer.alloc(size - at);
    readSync(fd, buf, 0, buf.length, at);
    closeSync(fd);
    at = size;
    seen += buf.length;
    out(buf.toString("utf8"));
  }, 200);
  const r = await exec(jlinkTool(dir, "JLinkRTTLogger", process.platform), rttArgs(device, log), false);
  clearInterval(pump);
  if (seen === 0) process.stderr.write(r.output);
}

if (process.argv[1] === fileURLToPath(import.meta.url)) {
  const env = hostJLinkEnv();
  const dir = findJLinkDir(env);
  if (dir === undefined) {
    process.stderr.write(`${notFoundMessage(env)}\n`);
    process.exit(1);
  }
  process.stdout.write(`${dir}\nversion ${jlinkVersion(dir) ?? "unknown"}\n`);
}
