/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { sleep } from "./usbFlash.ts";
import type { UsbHost } from "./usbFlash.ts";

const USB_CLASS_VENDOR = 0xff;
const USB_CLASS_APP = 0xfe;
const USB_SUBCLASS_DFU = 0x01;
const PACKET = 64;
const POLL_MS = 500;
const QUIET_HINT_MS = 3000;
const READ_TIMEOUT_MS = 24 * 60 * 60 * 1000;

interface LogDevice extends USBDevice {
  bus?: string;
  ports?: number[];
  transferIn(endpointNumber: number, length: number, timeout?: number): Promise<USBInTransferResult>;
}

export interface LogPort {
  interfaceNumber: number;
  inEndpoint: number;
  outEndpoint: number;
  packetSize: number;
}

export interface LogFilter {
  serial?: string;
  port?: string;
}

export interface StreamOptions {
  filter?: LogFilter;
  write?: (data: Uint8Array) => void;
}

export function utf8Writer(out: (text: string) => void): (data: Uint8Array) => void {
  const decoder = new TextDecoder();
  return (data) => out(decoder.decode(data, { stream: true }));
}

export function findLogPort(config: USBConfiguration | null | undefined): LogPort | undefined {
  if (!config) return undefined;
  const alts = (i: USBInterface) => (i.alternates.length > 0 ? i.alternates : [i.alternate]);
  const hasDfu = config.interfaces.some((i) =>
    alts(i).some((a) => a.interfaceClass === USB_CLASS_APP && a.interfaceSubclass === USB_SUBCLASS_DFU),
  );
  if (!hasDfu) return undefined;
  for (const i of config.interfaces) {
    const a = alts(i)[0];
    if (a?.interfaceClass !== USB_CLASS_VENDOR) continue;
    const bulkIn = a.endpoints.find((e) => e.type === "bulk" && e.direction === "in");
    const bulkOut = a.endpoints.find((e) => e.type === "bulk" && e.direction === "out");
    if (bulkIn && bulkOut) {
      return {
        interfaceNumber: i.interfaceNumber,
        inEndpoint: bulkIn.endpointNumber,
        outEndpoint: bulkOut.endpointNumber,
        packetSize: bulkIn.packetSize || PACKET,
      };
    }
  }
  return undefined;
}

export function chunks(data: Uint8Array, size: number = PACKET): Uint8Array<ArrayBuffer>[] {
  const out: Uint8Array<ArrayBuffer>[] = [];
  for (let at = 0; at < data.length; at += size) out.push(data.slice(at, at + size));
  return out;
}

function busNumber(bus: string): number {
  return /^\d+$/.test(bus) ? Number.parseInt(bus, 10) : Number.parseInt(bus, 16);
}

export function portPath(dev: { bus?: string; ports?: number[] }): string | undefined {
  if (dev.bus === undefined || !dev.ports?.length) return undefined;
  const bus = busNumber(dev.bus);
  return Number.isNaN(bus) ? undefined : `${bus}-${dev.ports.join(".")}`;
}

function serialOf(dev: USBDevice): string | undefined {
  try {
    return dev.serialNumber ?? undefined;
  } catch {
    return undefined;
  }
}

export function matchesFilter(
  dev: { serial?: string; bus?: string; ports?: number[] },
  filter: LogFilter | undefined,
): boolean {
  if (filter?.serial !== undefined && dev.serial?.toUpperCase() !== filter.serial.toUpperCase()) return false;
  if (filter?.port !== undefined && portPath(dev) !== filter.port) return false;
  return true;
}

export function describeFilter(filter: LogFilter | undefined): string {
  const parts = [
    filter?.serial !== undefined ? `serial ${filter.serial}` : "",
    filter?.port !== undefined ? `port ${filter.port}` : "",
  ].filter(Boolean);
  return parts.length ? parts.join(", ") : "any";
}

interface OpenLog {
  dev: LogDevice;
  port: LogPort;
  label: string;
}

function label(dev: LogDevice): string {
  const hex = (n: number) => n.toString(16).padStart(4, "0");
  const serial = serialOf(dev);
  const at = portPath(dev);
  return `${hex(dev.vendorId)}:${hex(dev.productId)}${serial ? ` ${serial}` : ""}${at ? ` at ${at}` : ""}`;
}

async function release(c: OpenLog): Promise<void> {
  await c.dev.releaseInterface(c.port.interfaceNumber).catch(() => undefined);
  await c.dev.close().catch(() => undefined);
}

async function openLog(host: UsbHost, filter: LogFilter | undefined): Promise<OpenLog | undefined> {
  for (const dev of (await host.getDevices().catch(() => [])) as LogDevice[]) {
    if (!matchesFilter({ serial: serialOf(dev), bus: dev.bus, ports: dev.ports }, filter)) continue;
    let port: LogPort | undefined;
    try {
      port = findLogPort(dev.configuration ?? dev.configurations[0]);
      if (!port) continue;
      if (!dev.opened) await dev.open();
      if (!dev.configuration) await dev.selectConfiguration(1);
      await dev.claimInterface(port.interfaceNumber);
      return { dev, port, label: label(dev) };
    } catch (e) {
      if (!port) continue;
      process.stderr.write(`USB log ${label(dev)}: ${(e as Error).message}\n`);
      await release({ dev, port, label: "" });
    }
  }
  return undefined;
}

export async function streamLog(host: UsbHost, options: StreamOptions = {}): Promise<never> {
  const write = options.write ?? ((data: Uint8Array) => process.stdout.write(data));
  let current: OpenLog | undefined;
  let pending = Promise.resolve();
  process.stdin.on("data", (chunk: Buffer) => {
    const target = current;
    if (!target) return;
    pending = pending
      .then(async () => {
        for (const c of chunks(chunk, target.port.packetSize)) await target.dev.transferOut(target.port.outEndpoint, c);
      })
      .catch((e: Error) => {
        process.stderr.write(`USB log send failed: ${e.message}\n`);
      });
  });

  let waited = false;
  for (;;) {
    current = await openLog(host, options.filter);
    if (!current) {
      if (!waited) {
        process.stderr.write(
          `\nwaiting for a module running a debug or test firmware over USB (${describeFilter(options.filter)})...\n`,
        );
      }
      waited = true;
      await sleep(POLL_MS);
      continue;
    }
    waited = false;
    process.stderr.write(`\nUSB log ${current.label} (ctrl-c to stop)\n`);
    let seen = false;
    const hint = setTimeout(() => {
      if (!seen) {
        process.stderr.write("no output yet; nothing has been printed, or a debug probe is reading the log\n");
      }
    }, QUIET_HINT_MS);
    try {
      for (;;) {
        const r = await current.dev.transferIn(current.port.inEndpoint, current.port.packetSize, READ_TIMEOUT_MS);
        if (r.data && r.data.byteLength > 0) {
          seen = true;
          write(new Uint8Array(r.data.buffer, r.data.byteOffset, r.data.byteLength));
        }
      }
    } catch (e) {
      process.stderr.write(`\nUSB log ${current.label} closed: ${(e as Error).message}\n`);
    } finally {
      clearTimeout(hint);
      await release(current);
      await sleep(POLL_MS);
      current = undefined;
    }
  }
}
