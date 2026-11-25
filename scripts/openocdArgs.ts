/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

export const RTT_SYMBOL = "_SEGGER_RTT";

export const RTT_SEARCH_SIZE = 1024;

export const RTT_PORT = 9090;

export const RTT_RETRY =
  "for {set i 0} {$i < 40} {incr i} { rtt start; if {![catch {rtt channels}]} break; sleep 250 }";

function targetArgs(target: string): string[] {
  return ["-f", "interface/stlink.cfg", "-f", target];
}

export function openocdLoadArgs(target: string, elf: string): string[] {
  return [...targetArgs(target), "-c", `program {${elf.replaceAll("\\", "/")}} verify reset exit`];
}

export function openocdRttArgs(target: string, address: string, port: number = RTT_PORT): string[] {
  return [
    ...targetArgs(target),
    "-c",
    "init",
    "-c",
    `rtt setup ${address} ${RTT_SEARCH_SIZE} "SEGGER RTT"`,
    "-c",
    RTT_RETRY,
    "-c",
    "rtt channels",
    "-c",
    `rtt server start ${port} 0`,
  ];
}

export function parseRttAddress(nmOutput: string): string | undefined {
  for (const line of nmOutput.split(/\r?\n/)) {
    const m = /^\s*([0-9a-fA-F]+)\s+\S\s+(\S+)\s*$/.exec(line);
    if (m !== null && m[2] === RTT_SYMBOL) return `0x${m[1]!.toLowerCase()}`;
  }
  return undefined;
}
