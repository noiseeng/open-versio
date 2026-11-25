/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { firmwareName } from "./buildArgs.ts";

export const JLINK_DOWNLOAD_URL = "https://www.segger.com/downloads/jlink/";

export const JLINK_TOOLS = ["JLink", "JLinkRTTLogger"] as const;

export interface JLinkHostSearch {
  roots: string[];
  fallbacks: string[];
}

export interface JLinkEnv {
  platform: string;
  listDir: (dir: string) => string[] | undefined;
  exists: (path: string) => boolean;
}

export function jlinkSuffix(platform: string): string {
  return platform === "win32" ? ".exe" : "Exe";
}

export function jlinkTool(dir: string, tool: string, platform: string): string {
  return `${dir.replace(/[\\/]+$/, "")}/${tool}${jlinkSuffix(platform)}`;
}

export function hostSearch(platform: string): JLinkHostSearch {
  if (platform === "win32")
    return { roots: ["C:/Program Files/SEGGER", "C:/Program Files (x86)/SEGGER"], fallbacks: [] };
  if (platform === "darwin") return { roots: ["/Applications/SEGGER"], fallbacks: [] };
  return { roots: ["/opt/SEGGER"], fallbacks: ["/usr/bin"] };
}

export function versionKey(name: string): [number, string] {
  const m = /v(\d+)([a-z]?)/i.exec(name);
  return m ? [Number(m[1]), (m[2] ?? "").toLowerCase()] : [0, ""];
}

export function sortJLinkDirs(names: string[]): string[] {
  return names
    .filter((name) => /^jlink/i.test(name))
    .sort((a, b) => {
      const [an, al] = versionKey(a);
      const [bn, bl] = versionKey(b);
      return bn - an || bl.localeCompare(al);
    });
}

export function candidateDirs(env: JLinkEnv): string[] {
  const search = hostSearch(env.platform);
  const found: string[] = [];
  for (const root of search.roots) {
    const listing = env.listDir(root);
    if (listing === undefined) continue;
    for (const name of sortJLinkDirs(listing)) found.push(`${root}/${name}`);
  }
  return [...found, ...search.fallbacks];
}

export function findJLinkDir(env: JLinkEnv): string | undefined {
  const has = (dir: string) => JLINK_TOOLS.every((tool) => env.exists(jlinkTool(dir, tool, env.platform)));
  return candidateDirs(env).find(has);
}

export function jlinkVersion(dir: string): string | undefined {
  const m = /v(\d+[a-z]?)/i.exec(dir.split(/[\\/]/).pop() ?? "");
  return m ? m[1] : undefined;
}

export function notFoundMessage(env: JLinkEnv): string {
  const search = hostSearch(env.platform);
  return [
    "no J-Link install found; searched:",
    ...search.roots.map((r) => `  ${r}/JLink*`),
    ...search.fallbacks.map((f) => `  ${f}`),
    `install J-Link to one of those from ${JLINK_DOWNLOAD_URL}`,
  ].join("\n");
}

export function flashCommands(elf: string): string {
  return `loadfile ${elf.replaceAll("\\", "/")}\nr\ng\nexit\n`;
}

export function flashArgs(device: string, commandFile: string): string[] {
  return [
    "-device",
    device,
    "-if",
    "SWD",
    "-speed",
    "4000",
    "-autoconnect",
    "1",
    "-NoGui",
    "1",
    "-ExitOnError",
    "1",
    "-CommandFile",
    commandFile,
  ];
}

export function rttArgs(device: string, log: string): string[] {
  return ["-Device", device, "-If", "SWD", "-Speed", "4000", "-RTTChannel", "0", log];
}

export function downloadConfirmed(output: string): boolean {
  return /Downloading file[\s\S]*?O\.K\./.test(output);
}

export interface RunCli {
  name: string;
  config: string;
  build: boolean;
  rtt: boolean;
  probe: Probe;
}

export const PROBES = ["usb", "jlink", "stlink"] as const;

export type Probe = (typeof PROBES)[number];

const RUN_CONFIGS = ["test", "debug", "ship"];
const RUN_USAGE = `usage: npm run debug -- [<name>] [${RUN_CONFIGS.join("|")}] [--probe ${PROBES.join("|")}] [--no-build] [--no-rtt]`;

function isProbe(p: string): p is Probe {
  return (PROBES as readonly string[]).includes(p);
}

export function parseRunCli(args: string[], firmwares: string[]): RunCli {
  let probe = "usb";
  const rest: string[] = [];
  for (let i = 0; i < args.length; i++) {
    const a = args[i]!;
    if (a === "--probe") {
      const v = args[++i];
      if (v === undefined) throw new Error(RUN_USAGE);
      probe = v;
    } else if (a.startsWith("--probe=")) probe = a.slice("--probe=".length);
    else rest.push(a);
  }
  if (!isProbe(probe)) throw new Error(`probe must be one of ${PROBES.join(", ")}, got ${probe}`);
  const positional = rest.filter((a) => !a.startsWith("--"));
  const unknown = rest.filter((a) => a.startsWith("--") && a !== "--no-build" && a !== "--no-rtt");
  if (unknown.length > 0) throw new Error(`unknown option ${unknown.join(", ")}`);
  if (positional.length > 2) throw new Error(RUN_USAGE);
  const configOnly = positional.length === 1 && RUN_CONFIGS.includes(positional[0]!);
  const config = (configOnly ? positional[0] : positional[1]) ?? "test";
  if (!RUN_CONFIGS.includes(config)) throw new Error(`config must be one of ${RUN_CONFIGS.join(", ")}, got ${config}`);
  return {
    name: firmwareName(firmwares, configOnly ? undefined : positional[0]),
    config,
    build: !rest.includes("--no-build"),
    rtt: !rest.includes("--no-rtt") && config !== "ship",
    probe,
  };
}
