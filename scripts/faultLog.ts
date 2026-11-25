/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { isAbsolute, relative } from "node:path";
import { exec } from "./exec.ts";
import { toolPath } from "./tools.ts";

export interface FaultQuery {
  address: number;
  interrupted: boolean;
}

export type Symbolize = (elf: string, addresses: number[]) => Promise<string[]>;

export interface FaultLogOptions {
  elf: string;
  root: string;
  hint: string;
  write: (text: string) => void;
  symbolize: Symbolize;
  settleMs: number;
}

const HEX = /^[0-9a-f]+$/i;

const CFSR_CAUSES: [number, string][] = [
  [0, "executed from non-executable memory"],
  [1, "memory access violation"],
  [3, "memory fault unstacking the exception"],
  [4, "memory fault stacking the exception (stack overflow?)"],
  [5, "memory fault saving the FPU state"],
  [8, "bus fault fetching an instruction"],
  [9, "bus fault"],
  [10, "bus fault on a buffered write (the pc is past the write)"],
  [11, "bus fault unstacking the exception"],
  [12, "bus fault stacking the exception (stack overflow?)"],
  [13, "bus fault saving the FPU state"],
  [16, "undefined instruction: a failed assert or bug::abort()"],
  [17, "invalid state: a call through a bad function pointer?"],
  [18, "invalid exception return"],
  [19, "FPU instruction with the FPU disabled"],
  [24, "unaligned access"],
  [25, "divide by zero"],
];

const MMARVALID = 7;
const BFARVALID = 15;
const HFSR_VECTTBL = 1;

function exact(value: number): number {
  return (value & ~1) >>> 0;
}

function returnAddress(value: number): number {
  return ((value & ~1) - 2) >>> 0;
}

function fields(line: string): Map<string, number> {
  const words = line.trim().split(/\s+/);
  const out = new Map<string, number>();
  for (let i = 1; i + 1 < words.length; i += 2) {
    if (HEX.test(words[i + 1]!)) out.set(words[i]!, parseInt(words[i + 1]!, 16) >>> 0);
  }
  return out;
}

function hex(value: number): string {
  return `0x${value.toString(16).padStart(8, "0")}`;
}

export function faultQueries(line: string): FaultQuery[] {
  const words = line.trim().split(/\s+/);
  const out: FaultQuery[] = [];
  if (words[0] === "hardfault") {
    const f = fields(line);
    const pc = f.get("pc");
    const lr = f.get("lr");
    if (pc !== undefined) out.push({ address: exact(pc), interrupted: false });
    if (lr !== undefined && (lr & 1) === 1 && lr < 0xffffffe0) {
      out.push({ address: returnAddress(lr), interrupted: false });
    }
  } else if (words[0] === "bt") {
    let interrupted = false;
    for (const w of words.slice(1)) {
      if (w === "|") {
        interrupted = true;
      } else if (HEX.test(w)) {
        const value = parseInt(w, 16);
        out.push(
          interrupted
            ? { address: exact(value), interrupted: true }
            : { address: returnAddress(value), interrupted: false },
        );
        interrupted = false;
      }
    }
  }
  return out;
}

export function faultSummary(line: string): string | undefined {
  if (line.trim().split(/\s+/)[0] !== "hardfault") return undefined;
  const f = fields(line);
  const cfsr = f.get("cfsr") ?? 0;
  const hfsr = f.get("hfsr") ?? 0;
  const causes: string[] = [];
  for (const [bit, text] of CFSR_CAUSES) {
    if ((cfsr & (1 << bit)) === 0) continue;
    if (bit === 1 && cfsr & (1 << MMARVALID)) causes.push(`${text} at ${hex(f.get("mmfar") ?? 0)}`);
    else if (bit === 9 && cfsr & (1 << BFARVALID)) causes.push(`${text} accessing ${hex(f.get("bfar") ?? 0)}`);
    else causes.push(text);
  }
  if (hfsr & (1 << HFSR_VECTTBL)) causes.push("fault reading the vector table");
  return `crash: ${causes.length > 0 ? causes.join("; ") : "hard fault"}`;
}

export function shortName(name: string): string {
  const space = name.indexOf(" ");
  const bracket = name.search(/[(<]/);
  const bare = space >= 0 && (bracket < 0 || space < bracket) ? name.slice(space + 1) : name;
  let out = "";
  let depth = 0;
  for (const c of bare) {
    if (c === "<") {
      if (depth === 0) out += "<…>";
      depth++;
    } else if (c === ">" && depth > 0) {
      depth--;
    } else if (depth === 0) {
      out += c;
    }
  }
  return collapseParams(out);
}

function collapseParams(name: string): string {
  const close = name.lastIndexOf(")");
  if (close < 0) return name;
  let depth = 0;
  for (let i = close; i >= 0; i--) {
    if (name[i] === ")") depth++;
    else if (name[i] === "(" && --depth === 0) {
      return i + 1 === close ? name : `${name.slice(0, i)}(…)${name.slice(close + 1)}`;
    }
  }
  return name;
}

function shortPath(location: string, root: string): string {
  const at = location.lastIndexOf(":");
  const file = at >= 0 ? location.slice(0, at) : location;
  const rel = isAbsolute(file) ? relative(root, file) : file;
  const shown = rel.startsWith("..") || isAbsolute(rel) ? file : rel;
  return at >= 0 ? `${shown}${location.slice(at)}` : shown;
}

const FRAME = /^(?:0x[0-9a-f]+: | \(inlined by\) )(?:(\?\?) \?\?:0|(.*) at (.*?)(?: \(discriminator \d+\))?)$/i;

export function formatFrames(resolved: string[], queries: FaultQuery[], root: string): string[] {
  const groups: string[][] = [];
  for (const line of resolved) {
    if (/^0x[0-9a-f]+: /i.test(line) || groups.length === 0) groups.push([line]);
    else groups[groups.length - 1]!.push(line);
  }
  const rows: [string, string][] = [];
  groups.forEach((group, i) => {
    group.forEach((line, j) => {
      const m = FRAME.exec(line);
      const name = !m ? line.trim() : m[1] ? "?? (not in the firmware)" : shortName(m[2]!);
      const where = m?.[3] ? shortPath(m[3], root) : "";
      rows.push([`${j === 0 && queries[i]?.interrupted ? "| interrupted: " : ""}${name}`, where]);
    });
  });
  const width = Math.min(60, Math.max(0, ...rows.map(([n]) => n.length)));
  return rows.map(([name, where]) => (where ? `  ${name.padEnd(width)}  ${where}` : `  ${name}`).trimEnd());
}

function dedupe(queries: FaultQuery[]): FaultQuery[] {
  return queries.filter((q, i) => i === 0 || q.address !== queries[i - 1]!.address || q.interrupted);
}

export function addr2line(exe: string): Symbolize {
  return async (elf, addresses) => {
    const args = ["-f", "-C", "-i", "-p", "-a", "-e", elf, ...addresses.map((a) => `0x${a.toString(16)}`)];
    const r = await exec(exe, args, false);
    if (r.code !== 0) throw new Error(`${exe} exited ${r.code}: ${r.output.trim()}`);
    return r.output.split(/\r?\n/).filter((l) => l.length > 0);
  };
}

export function toolsAddr2line(): Symbolize {
  return addr2line(toolPath("gcc", "bin/arm-none-eabi-addr2line"));
}

export function faultLog(opts: FaultLogOptions): (text: string) => void {
  let partial = "";
  let pending = Promise.resolve();
  let fault: { summary: string | undefined; queries: FaultQuery[] } | undefined;
  let timer: ReturnType<typeof setTimeout> | undefined;

  const report = (summary: string | undefined, raw: FaultQuery[]) => {
    const queries = dedupe(raw);
    pending = pending
      .then(async () => {
        const head = summary ? [summary, `  ${opts.hint}`] : [];
        const frames =
          queries.length > 0
            ? formatFrames(
                await opts.symbolize(
                  opts.elf,
                  queries.map((q) => q.address),
                ),
                queries,
                opts.root,
              )
            : [];
        opts.write([...head, ...frames].map((l) => `${l}\n`).join(""));
      })
      .catch((e: Error) => opts.write(`  addr2line failed: ${e.message}\n`));
  };

  const flush = () => {
    if (timer) clearTimeout(timer);
    timer = undefined;
    if (fault) report(fault.summary, fault.queries);
    fault = undefined;
  };

  return (text) => {
    opts.write(text);
    const lines = (partial + text).split("\n");
    partial = lines.pop() ?? "";
    for (const line of lines) {
      const word = line.trim().split(/\s+/)[0];
      if (word === "hardfault") {
        flush();
        fault = { summary: faultSummary(line), queries: faultQueries(line) };
        timer = setTimeout(flush, opts.settleMs);
      } else if (word === "bt") {
        const bt = faultQueries(line);
        if (fault) fault.queries.push(...bt);
        else fault = { summary: undefined, queries: bt };
        flush();
      } else {
        flush();
      }
    }
  };
}
