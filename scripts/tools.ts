/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { existsSync, mkdirSync, mkdtempSync, readFileSync, renameSync } from "node:fs";
import { join, win32 } from "node:path";
import { fileURLToPath } from "node:url";

export interface HostEntry {
  url: string;
  checksum: string;
}

export interface ToolEntry {
  version: string;
  bin: string;
  hosts: Record<string, HostEntry>;
}

export type ToolTable = Record<string, ToolEntry>;

export interface Checksum {
  algo: string;
  hex: string;
}

export const HOST_KEYS: Record<string, string> = {
  "win32/x64": "win-amd64",
  "darwin/x64": "mac-amd64",
  "darwin/arm64": "mac-arm64",
  "linux/x64": "lin-amd64",
  "linux/arm64": "lin-arm64",
};

export const REPO_ROOT = fileURLToPath(new URL("..", import.meta.url));

export function scratchDir(prefix: string): string {
  mkdirSync(join(REPO_ROOT, "tmp"), { recursive: true });
  return mkdtempSync(join(REPO_ROOT, "tmp", prefix));
}

export const TOOLS_JSON = join(REPO_ROOT, "tools.json");

export function hostKey(platform: string = process.platform, arch: string = process.arch): string {
  const key = HOST_KEYS[`${platform}/${arch}`];
  if (key === undefined) throw new Error(`unsupported platform ${platform}/${arch}`);
  return key;
}

export function loadTools(file: string = TOOLS_JSON): ToolTable {
  return JSON.parse(readFileSync(file, "utf8")) as ToolTable;
}

export function parseChecksum(text: string): Checksum {
  const m = /^([a-z0-9]+):([0-9a-fA-F]+)$/.exec(text);
  if (m === null) throw new Error(`bad checksum "${text}", expected <algo>:<hex>`);
  return { algo: m[1]!, hex: m[2]!.toLowerCase() };
}

export function toolDirName(tool: string, version: string): string {
  return `${tool}-${version}`;
}

export function tarPath(platform: string = process.platform, env: NodeJS.ProcessEnv = process.env): string {
  return platform === "win32" ? win32.join(env.SystemRoot ?? "C:\\Windows", "System32", "tar.exe") : "tar";
}

const BUSY_CODES = new Set(["EPERM", "EBUSY", "EACCES"]);

export async function renameRetrying(
  from: string,
  to: string,
  rename: (from: string, to: string) => void = renameSync,
  wait: (ms: number) => Promise<void> = (ms) => new Promise((resolve) => setTimeout(resolve, ms)),
  attempts = 20,
): Promise<void> {
  for (let attempt = 1; ; attempt++) {
    try {
      rename(from, to);
      return;
    } catch (e) {
      const code = (e as NodeJS.ErrnoException).code ?? "";
      if (!BUSY_CODES.has(code) || attempt === attempts) {
        throw new Error(
          `rename ${from} -> ${to} failed after ${attempt} attempt(s): ${(e as Error).message} (cwd ${process.cwd()})`,
        );
      }
      await wait(250 * attempt);
    }
  }
}

export function toolPath(
  tool: string,
  relBin: string,
  root: string = REPO_ROOT,
  table: ToolTable = loadTools(join(root, "tools.json")),
  platform: string = process.platform,
): string {
  const entry = table[tool];
  if (entry === undefined) throw new Error(`unknown tool ${tool} (not in ${join(root, "tools.json")})`);
  const dir = join(root, "tools", toolDirName(tool, entry.version));
  if (!existsSync(join(dir, ".installed")))
    throw new Error(`${tool} ${entry.version} is not installed: run npm run setup`);
  return join(dir, platform === "win32" ? `${relBin}.exe` : relBin);
}
