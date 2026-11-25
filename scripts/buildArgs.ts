/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { readFileSync } from "node:fs";
import { basename, join } from "node:path";

export interface FirmwareConfig {
  name: string;
  sources: string[];
}

export interface ConfigVariant {
  defines: string[];
  compileFlags: string[];
  extraSources: string[];
}

export interface BuildConfig {
  jlinkDevice: string;
  openocdTarget: string;
  includeDirs: string[];
  defines: string[];
  std: string;
  cpuFlags: string[];
  compileFlags: string[];
  linkerScript: string;
  linkFlags: string[];
  linkLibs: string[];
  configs: Record<string, ConfigVariant>;
  firmwares: FirmwareConfig[];
}

export const DEFAULT_CONFIG = "ship";

export function loadBuildConfig(root: string): BuildConfig {
  return JSON.parse(readFileSync(join(root, "build.json"), "utf8")) as BuildConfig;
}

export function firmwareName(firmwares: string[], name: string | undefined): string {
  if (name !== undefined) return name;
  if (firmwares.length !== 1) throw new Error(`name a firmware; build.json lists ${firmwares.join(", ")}`);
  return firmwares[0]!;
}

export function variantOf(config: BuildConfig, name: string): ConfigVariant {
  const variant = config.configs[name];
  if (variant === undefined)
    throw new Error(`unknown config ${name}; build.json lists ${Object.keys(config.configs).join(", ")}`);
  return variant;
}

export function sourcesOf(config: BuildConfig, name: string, firmware: FirmwareConfig): string[] {
  return [...firmware.sources, ...variantOf(config, name).extraSources];
}

export function buildDir(name: string, firmware: FirmwareConfig): string {
  return `build/${name}/${firmware.name}`;
}

export function objectPath(name: string, firmware: FirmwareConfig, source: string): string {
  return `${buildDir(name, firmware)}/${basename(source)}.o`;
}

export function elfPath(name: string, firmware: FirmwareConfig): string {
  return `${buildDir(name, firmware)}/${firmware.name}.elf`;
}

export function mapPath(name: string, firmware: FirmwareConfig): string {
  return `${buildDir(name, firmware)}/${firmware.name}.map`;
}

export function binPath(name: string, firmware: FirmwareConfig): string {
  return `${buildDir(name, firmware)}/${firmware.name}.bin`;
}

export function compileArgs(config: BuildConfig, name: string, firmware: FirmwareConfig, source: string): string[] {
  const variant = variantOf(config, name);
  return [
    `-std=${config.std}`,
    ...[...config.defines, ...variant.defines].map((d) => `-D${d}`),
    ...config.cpuFlags,
    ...config.compileFlags,
    ...variant.compileFlags,
    ...config.includeDirs.flatMap((d) => ["-I", d]),
    "-MD",
    "-MP",
    "-c",
    source,
    "-o",
    objectPath(name, firmware, source),
  ];
}

export interface CompileCommand {
  directory: string;
  file: string;
  arguments: string[];
}

export function parseSystemIncludes(verbose: string): string[] {
  const lines = verbose.split(/\r?\n/);
  const start = lines.findIndex((l) => l.startsWith("#include <...> search starts here:"));
  const end = lines.findIndex((l) => l.startsWith("End of search list."));
  if (start < 0 || end < start) throw new Error(`no #include <...> search list in compiler output:\n${verbose}`);
  return lines.slice(start + 1, end).map((l) => l.trim());
}

export function compileCommands(
  root: string,
  config: BuildConfig,
  name: string,
  firmware: FirmwareConfig,
  compiler: { c: string; cxx: string },
  systemIncludes: string[],
): CompileCommand[] {
  return sourcesOf(config, name, firmware).map((source) => ({
    directory: root,
    file: join(root, source),
    arguments: [
      source.endsWith(".c") ? compiler.c : compiler.cxx,
      ...compileArgs(config, name, firmware, source),
      ...systemIncludes.flatMap((d) => ["-isystem", d]),
    ],
  }));
}

export function linkArgs(config: BuildConfig, name: string, firmware: FirmwareConfig): string[] {
  return [
    ...config.cpuFlags,
    ...config.linkFlags,
    `-T${config.linkerScript}`,
    `-Wl,-Map,${mapPath(name, firmware)}`,
    ...sourcesOf(config, name, firmware).map((s) => objectPath(name, firmware, s)),
    ...config.linkLibs,
    "-o",
    elfPath(name, firmware),
  ];
}

export function objcopyArgs(name: string, firmware: FirmwareConfig): string[] {
  return ["-O", "binary", elfPath(name, firmware), binPath(name, firmware)];
}

export function sizeArgs(name: string, firmware: FirmwareConfig): string[] {
  return [elfPath(name, firmware)];
}

export interface BuildCli {
  config: string;
  names: string[];
}

export function parseBuildCli(args: string[]): BuildCli {
  let config = DEFAULT_CONFIG;
  const names: string[] = [];
  for (let i = 0; i < args.length; i++) {
    const a = args[i]!;
    if (a === "--config") {
      const value = args[++i];
      if (value === undefined) throw new Error("--config needs a value");
      config = value;
    } else if (a.startsWith("--config=")) config = a.slice("--config=".length);
    else if (a.startsWith("--")) throw new Error(`unknown option ${a}`);
    else names.push(a);
  }
  return { config, names };
}
