/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

export interface Segment {
  start: number;
  end: number;
  sectorSize: number;
  readable: boolean;
  erasable: boolean;
  writable: boolean;
}

const SIZE_UNITS: Record<string, number> = { " ": 1, B: 1, K: 1024, M: 1048576 };

export function parseLayout(desc: string): Segment[] {
  const slash = desc.indexOf("/");
  if (!desc.startsWith("@") || slash === -1) return [];
  const rest = desc.substring(slash);
  const regionRx = /\/\s*(0x[0-9a-fA-F]{1,8})\s*\/(\s*\d+\s*\*\s*\d+\s?[ BKM]\s*[abcdefg]\s*,?\s*)+/g;
  const runRx = /(\d+)\s*\*\s*(\d+)\s?([ BKM])\s*([abcdefg])\s*,?\s*/g;
  const segments: Segment[] = [];
  for (const region of rest.matchAll(regionRx)) {
    let addr = parseInt(region[1]!, 16);
    for (const run of region[0].matchAll(runRx)) {
      const count = parseInt(run[1]!, 10);
      const size = parseInt(run[2]!, 10) * (SIZE_UNITS[run[3]!] ?? 1);
      const perms = run[4]!.charCodeAt(0) - 97 + 1;
      segments.push({
        start: addr,
        end: addr + size * count,
        sectorSize: size,
        readable: (perms & 1) !== 0,
        erasable: (perms & 2) !== 0,
        writable: (perms & 4) !== 0,
      });
      addr += size * count;
    }
  }
  return segments;
}

export function segmentAt(segments: Segment[], addr: number): Segment | undefined {
  return segments.find((s) => addr >= s.start && addr < s.end);
}

export function sectorsToErase(segments: Segment[], address: number, length: number): number[] {
  const sectors: number[] = [];
  const end = address + length;
  let addr = address;
  while (addr < end) {
    const seg = segmentAt(segments, addr);
    if (!seg || !seg.erasable) {
      throw new Error(
        `0x${addr.toString(16)} is not in an erasable region of the device layout; ` +
          `cannot write 0x${address.toString(16)}..0x${end.toString(16)}`,
      );
    }
    const sector = seg.start + Math.floor((addr - seg.start) / seg.sectorSize) * seg.sectorSize;
    sectors.push(sector);
    addr = sector + seg.sectorSize;
  }
  return sectors;
}

export function isRuntimeDfu(protocols: number[]): boolean {
  return protocols.includes(1) && !protocols.includes(2);
}

export interface ImageComparison {
  ok: boolean;
  firstDiff: number;
  mismatches: number;
}

export function compareImages(written: Uint8Array, readback: Uint8Array): ImageComparison {
  const n = Math.min(written.length, readback.length);
  let firstDiff = -1;
  let mismatches = 0;
  for (let i = 0; i < n; i++) {
    if (written[i] !== readback[i]) {
      if (firstDiff < 0) firstDiff = i;
      mismatches++;
    }
  }
  if (readback.length < written.length) {
    if (firstDiff < 0) firstDiff = readback.length;
    mismatches += written.length - readback.length;
  }
  return { ok: mismatches === 0, firstDiff, mismatches };
}
