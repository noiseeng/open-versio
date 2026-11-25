/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { spawn, spawnSync } from "node:child_process";
import { mkdirSync, writeFileSync } from "node:fs";
import { join } from "node:path";
import {
  binPath,
  buildDir,
  compileArgs,
  compileCommands,
  linkArgs,
  loadBuildConfig,
  objcopyArgs,
  parseBuildCli,
  parseSystemIncludes,
  sizeArgs,
  sourcesOf,
  variantOf,
} from "./buildArgs.ts";
import type { BuildConfig, CompileCommand, FirmwareConfig } from "./buildArgs.ts";
import { REPO_ROOT, toolPath } from "./tools.ts";

interface RunResult {
  code: number;
  output: string;
}

function run(exe: string, args: string[]): Promise<RunResult> {
  return new Promise((resolve) => {
    const child = spawn(exe, args, { cwd: REPO_ROOT });
    let output = "";
    child.stdout.on("data", (d) => (output += String(d)));
    child.stderr.on("data", (d) => (output += String(d)));
    child.on("error", (e) => resolve({ code: 1, output: `${exe}: ${e.message} (cwd ${REPO_ROOT})\n` }));
    child.on("close", (code) => resolve({ code: code ?? 1, output }));
  });
}

async function step(exe: string, args: string[]): Promise<string> {
  const result = await run(exe, args);
  if (result.code !== 0) {
    process.stderr.write(`${exe} ${args.join(" ")}\n${result.output}`);
    process.exit(1);
  }
  return result.output;
}

async function buildFirmware(config: BuildConfig, name: string, firmware: FirmwareConfig): Promise<void> {
  const gcc = toolPath("gcc", "bin/arm-none-eabi-gcc");
  const gxx = toolPath("gcc", "bin/arm-none-eabi-g++");
  const objcopy = toolPath("gcc", "bin/arm-none-eabi-objcopy");
  const size = toolPath("gcc", "bin/arm-none-eabi-size");
  mkdirSync(join(REPO_ROOT, buildDir(name, firmware)), { recursive: true });
  await Promise.all(
    sourcesOf(config, name, firmware).map(async (source) => {
      const out = await step(source.endsWith(".c") ? gcc : gxx, compileArgs(config, name, firmware, source));
      process.stdout.write(out);
    }),
  );
  process.stdout.write(await step(gxx, linkArgs(config, name, firmware)));
  await step(objcopy, objcopyArgs(name, firmware));
  process.stdout.write(await step(size, sizeArgs(name, firmware)));
  process.stdout.write(`${binPath(name, firmware)}\n`);
}

function systemIncludes(config: BuildConfig, cxx: string): string[] {
  const r = spawnSync(cxx, [`-std=${config.std}`, ...config.cpuFlags, "-xc++", "-E", "-v", "-"], {
    cwd: REPO_ROOT,
    input: "",
    encoding: "utf8",
  });
  if (r.status !== 0) throw new Error(`${cxx} -E -v failed with status ${r.status}: ${r.stderr} (cwd ${REPO_ROOT})`);
  return parseSystemIncludes(r.stderr);
}

function writeCompileCommands(config: BuildConfig, name: string, firmwares: FirmwareConfig[]): void {
  const compiler = { c: toolPath("gcc", "bin/arm-none-eabi-gcc"), cxx: toolPath("gcc", "bin/arm-none-eabi-g++") };
  const includes = systemIncludes(config, compiler.cxx);
  const commands: CompileCommand[] = firmwares.flatMap((f) =>
    compileCommands(REPO_ROOT, config, name, f, compiler, includes),
  );
  writeFileSync(join(REPO_ROOT, "compile_commands.json"), `${JSON.stringify(commands, null, 2)}\n`);
}

async function main(): Promise<void> {
  const config = loadBuildConfig(REPO_ROOT);
  const cli = parseBuildCli(process.argv.slice(2));
  variantOf(config, cli.config);
  const unknown = cli.names.filter((n) => !config.firmwares.some((f) => f.name === n));
  if (unknown.length > 0) {
    process.stderr.write(
      `unknown firmware ${unknown.join(", ")}; build.json lists ${config.firmwares.map((f) => f.name).join(", ")}\n`,
    );
    process.exit(1);
  }
  writeCompileCommands(config, cli.config, config.firmwares);
  for (const firmware of config.firmwares)
    if (cli.names.length === 0 || cli.names.includes(firmware.name)) await buildFirmware(config, cli.config, firmware);
}

await main();
