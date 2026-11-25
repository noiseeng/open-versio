/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { spawnSync } from "node:child_process";
import { createHash } from "node:crypto";
import {
  createReadStream,
  createWriteStream,
  existsSync,
  mkdirSync,
  readdirSync,
  rmSync,
  writeFileSync,
} from "node:fs";
import { join } from "node:path";
import { Readable } from "node:stream";
import { pipeline } from "node:stream/promises";
import { hostKey, loadTools, parseChecksum, REPO_ROOT, renameRetrying, tarPath, toolDirName } from "./tools.ts";
import type { HostEntry } from "./tools.ts";

const TOOLS = join(REPO_ROOT, "tools");
const DOWNLOAD = join(TOOLS, ".download");
const RM_RETRY = { recursive: true, force: true, maxRetries: 10, retryDelay: 250 } as const;

async function hashFile(file: string, algo: string): Promise<string> {
  const h = createHash(algo);
  await pipeline(createReadStream(file), h);
  return h.digest("hex");
}

async function verified(host: HostEntry, file: string): Promise<boolean> {
  const sum = parseChecksum(host.checksum);
  return existsSync(file) && (await hashFile(file, sum.algo)) === sum.hex;
}

async function download(host: HostEntry, file: string): Promise<void> {
  if (await verified(host, file)) {
    console.log(`  reusing ${file}`);
    return;
  }
  const res = await fetch(host.url);
  if (!res.ok || res.body === null)
    throw new Error(`download ${host.url} failed: HTTP ${res.status} (cwd ${process.cwd()})`);
  await pipeline(Readable.fromWeb(res.body as never), createWriteStream(file));
  if (!(await verified(host, file))) {
    rmSync(file, { force: true });
    throw new Error(`checksum mismatch for ${host.url}: expected ${parseChecksum(host.checksum).hex}`);
  }
}

function extract(archive: string, stage: string): void {
  rmSync(stage, RM_RETRY);
  mkdirSync(stage, { recursive: true });
  const tar = tarPath();
  const r = spawnSync(tar, ["-xf", archive, "-C", stage], { stdio: "inherit" });
  if (r.status !== 0)
    throw new Error(`${tar} -xf ${archive} failed with status ${r.status} ${r.error ?? ""} (cwd ${process.cwd()})`);
}

async function install(name: string): Promise<void> {
  const table = loadTools();
  const entry = table[name];
  if (entry === undefined) throw new Error(`unknown tool ${name}; known: ${Object.keys(table).join(", ")}`);
  const dest = join(TOOLS, toolDirName(name, entry.version));
  if (existsSync(join(dest, ".installed"))) {
    console.log(`${name} ${entry.version}: already installed`);
    return;
  }
  const key = hostKey();
  const host = entry.hosts[key];
  if (host === undefined) throw new Error(`${name} ${entry.version} has no download for ${key}`);
  mkdirSync(DOWNLOAD, { recursive: true });
  const archive = join(DOWNLOAD, host.url.split("/").pop()!);
  console.log(`${name} ${entry.version}: downloading ${host.url}`);
  await download(host, archive);
  const stage = join(DOWNLOAD, `${toolDirName(name, entry.version)}.stage`);
  try {
    extract(archive, stage);
    const top = readdirSync(stage);
    const root = top.length === 1 ? join(stage, top[0]!) : stage;
    rmSync(dest, RM_RETRY);
    await renameRetrying(root, dest);
  } finally {
    rmSync(stage, RM_RETRY);
  }
  rmSync(archive, { force: true });
  writeFileSync(join(dest, ".installed"), `${entry.version}\n`);
  console.log(`${name} ${entry.version}: installed in ${dest}`);
}

const table = loadTools();
const named = process.argv.slice(2);
const names = named.length > 0 ? named : Object.keys(table);
for (const n of names) await install(n);
