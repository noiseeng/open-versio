/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { existsSync, readFileSync } from "node:fs";
import { join } from "node:path";
import { fileURLToPath } from "node:url";
import { binPath } from "./buildArgs.ts";
import { compareImages, isRuntimeDfu, parseLayout, sectorsToErase, segmentAt } from "./dfuse.ts";
import type { Segment } from "./dfuse.ts";

const REPO_ROOT = fileURLToPath(new URL("..", import.meta.url));
const FLASH_BASE = 0x08000000;
const CONTROL_TIMEOUT_MS = 5000;
const REENUMERATE_TIMEOUT_MS = 15_000;

const RT_STD_DEV_IN = 0x80;
const RT_CLASS_IFACE_OUT = 0x21;
const RT_CLASS_IFACE_IN = 0xa1;
const GET_DESCRIPTOR = 0x06;
const DT_CONFIGURATION = 0x02;
const DT_STRING = 0x03;
const DT_INTERFACE = 0x04;
const DT_DFU_FUNCTIONAL = 0x21;
const USB_CLASS_APP = 0xfe;
const USB_SUBCLASS_DFU = 0x01;

const DFU_DETACH = 0x00;
const DFU_DOWNLOAD = 0x01;
const DFU_UPLOAD = 0x02;
const DFU_GETSTATUS = 0x03;
const DFU_CLRSTATUS = 0x04;
const DFU_GETSTATE = 0x05;
const DFU_ABORT = 0x06;

const ST_IDLE = 2;
const ST_DNLOAD_IDLE = 5;
const ST_DNLOAD_BUSY = 4;
const ST_ERROR = 10;
const STATUS_OK = 0;

const DFUSE_SET_ADDRESS = 0x21;
const DFUSE_ERASE_SECTOR = 0x41;

interface UsbDevice extends USBDevice {
  bus?: string;
  address?: number;
  controlTransferIn(
    setup: USBControlTransferParameters,
    length: number,
    timeout?: number,
  ): Promise<USBInTransferResult>;
  controlTransferOut(
    setup: USBControlTransferParameters,
    data?: BufferSource,
    timeout?: number,
  ): Promise<USBOutTransferResult>;
}

export interface UsbHost {
  getDevices(): Promise<USBDevice[]>;
}

interface DfuInterface {
  interfaceNumber: number;
  altSetting: number;
  protocol: number;
  name: string;
  segments: Segment[];
  transferSize: number;
}

export function sleep(ms: number): Promise<void> {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

function hex4(n: number): string {
  return n.toString(16).padStart(4, "0");
}

function setup(bmType: number, request: number, value: number, index: number): USBControlTransferParameters {
  const types = ["standard", "class", "vendor"] as const;
  const recipients = ["device", "interface", "endpoint", "other"] as const;
  return {
    requestType: types[(bmType >> 5) & 3] ?? "vendor",
    recipient: recipients[bmType & 0x1f] ?? "other",
    request,
    value,
    index,
  };
}

export async function loadUsb(): Promise<UsbHost> {
  try {
    const { WebUSB } = await import("usb");
    return new WebUSB({ allowAllDevices: true });
  } catch (e) {
    if ((e as NodeJS.ErrnoException).code === "ERR_MODULE_NOT_FOUND") {
      throw new Error(`the "usb" module is not installed: run "npm install" in ${REPO_ROOT}, then retry`);
    }
    throw e;
  }
}

export class DfuDevice {
  readonly dev: UsbDevice;
  interfaces: DfuInterface[] = [];
  active: DfuInterface | undefined;

  constructor(dev: UsbDevice) {
    this.dev = dev;
  }

  get label(): string {
    return `${hex4(this.dev.vendorId)}:${hex4(this.dev.productId)}`;
  }

  get runtime(): boolean {
    return isRuntimeDfu(this.interfaces.map((i) => i.protocol));
  }

  async open(): Promise<void> {
    await this.dev.open();
    if (!this.dev.configuration) await this.dev.selectConfiguration(1);
    const dfu = this.dev.configuration?.interfaces.find((i) =>
      i.alternates.some((a) => a.interfaceClass === USB_CLASS_APP && a.interfaceSubclass === USB_SUBCLASS_DFU),
    );
    if (!dfu) return;
    await this.dev.claimInterface(dfu.interfaceNumber);
    const header = await this.ctrlIn(RT_STD_DEV_IN, GET_DESCRIPTOR, DT_CONFIGURATION << 8, 0, 4);
    const raw = await this.ctrlIn(RT_STD_DEV_IN, GET_DESCRIPTOR, DT_CONFIGURATION << 8, 0, header.readUInt16LE(2));
    const found: Array<{ iface: number; alt: number; protocol: number; stringIndex: number; transferSize: number }> =
      [];
    let off = 9;
    while (off + 2 <= raw.length && raw[off]! !== 0) {
      const len = raw[off]!;
      const type = raw[off + 1]!;
      if (type === DT_INTERFACE && len >= 9) {
        const isDfu = raw[off + 5] === USB_CLASS_APP && raw[off + 6] === USB_SUBCLASS_DFU;
        const protocol = raw[off + 7]!;
        if (isDfu && (protocol === 1 || protocol === 2)) {
          found.push({
            iface: raw[off + 2]!,
            alt: raw[off + 3]!,
            protocol,
            stringIndex: raw[off + 8]!,
            transferSize: 0,
          });
        }
      } else if (type === DT_DFU_FUNCTIONAL && len >= 9 && found.length > 0) {
        found[found.length - 1]!.transferSize = raw.readUInt16LE(off + 5);
      }
      off += len;
    }
    this.interfaces = [];
    for (const f of found) {
      const name = f.stringIndex > 0 ? await this.readString(f.stringIndex) : "";
      this.interfaces.push({
        interfaceNumber: f.iface,
        altSetting: f.alt,
        protocol: f.protocol,
        name,
        segments: parseLayout(name),
        transferSize: f.transferSize || 1024,
      });
    }
  }

  close(): void {
    void this.dev.close().catch(() => undefined);
  }

  private async readString(index: number): Promise<string> {
    try {
      const value = (DT_STRING << 8) | index;
      const head = await this.ctrlIn(RT_STD_DEV_IN, GET_DESCRIPTOR, value, 0x0409, 2);
      const full = await this.ctrlIn(RT_STD_DEV_IN, GET_DESCRIPTOR, value, 0x0409, head[0]!);
      return full.subarray(2).toString("utf16le");
    } catch {
      return "";
    }
  }

  private async ctrlIn(bmType: number, request: number, value: number, index: number, length: number): Promise<Buffer> {
    const r = await this.dev.controlTransferIn(setup(bmType, request, value, index), length, CONTROL_TIMEOUT_MS);
    if (r.status !== "ok" || !r.data) throw new Error(`control IN 0x${request.toString(16)}: ${r.status}`);
    return Buffer.from(r.data.buffer, r.data.byteOffset, r.data.byteLength);
  }

  private async ctrlOut(bmType: number, request: number, value: number, index: number, data: Buffer): Promise<void> {
    const r = await this.dev.controlTransferOut(
      setup(bmType, request, value, index),
      data as BufferSource,
      CONTROL_TIMEOUT_MS,
    );
    if (r.status !== "ok") throw new Error(`control OUT 0x${request.toString(16)}: ${r.status}`);
  }

  private get iface(): number {
    if (!this.active) throw new Error(`no DFU interface selected on ${this.label}`);
    return this.active.interfaceNumber;
  }

  private dfuIn(request: number, value: number, length: number): Promise<Buffer> {
    return this.ctrlIn(RT_CLASS_IFACE_IN, request, value, this.iface, length);
  }

  private dfuOut(request: number, value: number, data: Buffer): Promise<void> {
    return this.ctrlOut(RT_CLASS_IFACE_OUT, request, value, this.iface, data);
  }

  async claim(target: DfuInterface): Promise<void> {
    this.active = target;
    const iface = this.dev.configuration?.interfaces.find((i) => i.interfaceNumber === target.interfaceNumber);
    if (!iface?.claimed) await this.dev.claimInterface(target.interfaceNumber);
    await this.dev.selectAlternateInterface(target.interfaceNumber, target.altSetting);
  }

  async detach(): Promise<void> {
    await this.dfuOut(DFU_DETACH, 1000, Buffer.alloc(0));
  }

  private async status(): Promise<{ status: number; pollTimeout: number; state: number }> {
    const d = await this.dfuIn(DFU_GETSTATUS, 0, 6);
    return { status: d[0]!, pollTimeout: d.readUInt32LE(1) & 0xffffff, state: d[4]! };
  }

  private async state(): Promise<number> {
    return (await this.dfuIn(DFU_GETSTATE, 0, 1))[0]!;
  }

  private clearStatus(): Promise<void> {
    return this.dfuOut(DFU_CLRSTATUS, 0, Buffer.alloc(0));
  }

  async toIdle(): Promise<void> {
    if ((await this.state()) === ST_IDLE) return;
    await this.dfuOut(DFU_ABORT, 0, Buffer.alloc(0));
    for (let i = 0; i < 5 && (await this.state()) !== ST_IDLE; i++) {
      try {
        await this.clearStatus();
      } catch {
        void 0;
      }
      await sleep(10);
    }
    const state = await this.state();
    if (state !== ST_IDLE) throw new Error(`device ${this.label} would not return to dfuIDLE (state ${state})`);
  }

  private async pollUntil(done: (state: number) => boolean, timeoutMs: number, what: string): Promise<void> {
    const start = Date.now();
    let s = await this.status();
    while (!done(s.state)) {
      if (s.state === ST_ERROR) throw new Error(`${what}: device reported dfuERROR (status ${s.status})`);
      const elapsed = Date.now() - start;
      if (elapsed > timeoutMs) throw new Error(`${what}: still in state ${s.state} after ${timeoutMs}ms`);
      if (s.state === ST_DNLOAD_BUSY && elapsed >= 4000 && elapsed % 4000 < 50) {
        try {
          await this.clearStatus();
        } catch {
          void 0;
        }
      }
      await sleep(s.pollTimeout || 1);
      s = await this.status();
    }
    if (s.status !== STATUS_OK) throw new Error(`${what}: device status ${s.status} in state ${s.state}`);
  }

  private async download(data: Buffer, block: number): Promise<void> {
    for (;;) {
      const s = await this.status();
      if (s.state === ST_IDLE || s.state === ST_DNLOAD_IDLE) break;
      if (s.state === ST_DNLOAD_BUSY) {
        await this.pollUntil((state) => state !== ST_DNLOAD_BUSY, 30_000, "download");
        continue;
      }
      await this.clearStatus();
    }
    await this.dfuOut(DFU_DOWNLOAD, block, data);
  }

  private async command(command: number, address: number): Promise<void> {
    const payload = Buffer.alloc(5);
    payload[0] = command;
    payload.writeUInt32LE(address, 1);
    await this.download(payload, 0);
    const erase = command === DFUSE_ERASE_SECTOR;
    await this.pollUntil(
      erase ? (s) => s === ST_DNLOAD_IDLE || s === ST_IDLE : (s) => s !== ST_DNLOAD_BUSY,
      erase ? 120_000 : 30_000,
      `DfuSe command 0x${command.toString(16)} at 0x${address.toString(16)}`,
    );
  }

  async read(address: number, length: number): Promise<Buffer> {
    const xfer = this.active?.transferSize ?? 1024;
    await this.toIdle();
    await this.command(DFUSE_SET_ADDRESS, address);
    await this.toIdle();
    const chunks: Buffer[] = [];
    let remaining = length;
    let block = 2;
    while (remaining > 0) {
      const want = Math.min(remaining, xfer);
      const chunk = await this.dfuIn(DFU_UPLOAD, block, want);
      if (chunk.length === 0) break;
      chunks.push(chunk);
      remaining -= chunk.length;
      block++;
      if (chunk.length < want) break;
    }
    return Buffer.concat(chunks);
  }

  async write(address: number, image: Buffer): Promise<void> {
    const target = this.active;
    if (!target) throw new Error(`no DFU interface selected on ${this.label}`);
    const sectors = sectorsToErase(target.segments, address, image.length);
    if ((await this.state()) === ST_ERROR) await this.clearStatus();
    for (const [i, sector] of sectors.entries()) {
      process.stdout.write(`  erasing ${i + 1}/${sectors.length} (0x${sector.toString(16)})\r`);
      await this.command(DFUSE_ERASE_SECTOR, sector);
    }
    process.stdout.write("\n");
    await this.command(DFUSE_SET_ADDRESS, address);
    let block = 2;
    for (let off = 0; off < image.length; off += target.transferSize, block++) {
      await this.download(image.subarray(off, off + target.transferSize), block);
      await this.pollUntil((s) => s === ST_DNLOAD_IDLE, 30_000, `write at 0x${(address + off).toString(16)}`);
      process.stdout.write(`  writing ${Math.min(off + target.transferSize, image.length)}/${image.length}\r`);
    }
    process.stdout.write("\n");
  }

  async verify(address: number, image: Buffer): Promise<void> {
    await this.read(address, 16);
    const readback = await this.read(address, image.length);
    const cmp = compareImages(image, readback);
    if (!cmp.ok) {
      throw new Error(
        `verify failed: ${cmp.mismatches} byte(s) differ, first at 0x${(address + cmp.firstDiff).toString(16)}`,
      );
    }
  }

  async leave(address: number): Promise<void> {
    await this.command(DFUSE_SET_ADDRESS, address);
    await this.download(Buffer.alloc(0), 2);
    try {
      await this.status();
      await this.status();
    } catch {
      void 0;
    }
  }
}

async function openCandidates(host: UsbHost): Promise<DfuDevice[]> {
  const opened: DfuDevice[] = [];
  for (const dev of await host.getDevices()) {
    const dfu = new DfuDevice(dev as UsbDevice);
    try {
      await dfu.open();
    } catch {
      dfu.close();
      continue;
    }
    if (dfu.interfaces.length === 0) {
      dfu.close();
      continue;
    }
    opened.push(dfu);
  }
  return opened;
}

function flashInterface(dfu: DfuDevice): DfuInterface | undefined {
  return dfu.interfaces.find((i) => segmentAt(i.segments, FLASH_BASE)?.erasable);
}

export async function listDevices(host: UsbHost): Promise<void> {
  const devices = await openCandidates(host);
  if (devices.length === 0) {
    console.log("no DFU devices found");
    return;
  }
  for (const dfu of devices) {
    console.log(`${dfu.label} ${dfu.runtime ? "runtime DFU (running firmware)" : "DFU bootloader"}`);
    for (const i of dfu.interfaces) console.log(`  alt ${i.altSetting}: ${i.name || "(unnamed)"}`);
    dfu.close();
  }
}

async function waitForBootloader(host: UsbHost): Promise<DfuDevice> {
  const deadline = Date.now() + REENUMERATE_TIMEOUT_MS;
  while (Date.now() < deadline) {
    await sleep(500);
    for (const dfu of await openCandidates(host)) {
      if (!dfu.runtime && flashInterface(dfu)) return dfu;
      dfu.close();
    }
  }
  throw new Error(
    `no DFU bootloader with internal flash at 0x${FLASH_BASE.toString(16)} appeared within ` +
      `${REENUMERATE_TIMEOUT_MS / 1000}s after detach`,
  );
}

async function findFlashTarget(host: UsbHost): Promise<DfuDevice> {
  const all = await openCandidates(host);
  const bootloader = all.find((d) => !d.runtime && flashInterface(d));
  const runtime = bootloader ? undefined : all.find((d) => d.runtime);
  for (const d of all) if (d !== bootloader && d !== runtime) d.close();
  if (bootloader) return bootloader;
  if (!runtime) {
    throw new Error(
      `no DFU device with internal flash at 0x${FLASH_BASE.toString(16)} found: connect the module over USB ` +
        `(or hold its bootloader button) and retry; --list shows what is visible`,
    );
  }
  console.log(`detaching ${runtime.label} into its bootloader...`);
  await runtime.claim(runtime.interfaces.find((i) => i.protocol === 1)!);
  try {
    await runtime.detach();
  } catch {
    void 0;
  }
  runtime.close();
  return waitForBootloader(host);
}

export function findImage(config: string, name: string): string {
  const bin = join(REPO_ROOT, binPath(config, { name, sources: [] }));
  if (!existsSync(bin)) throw new Error(`${bin} not found; run "npm run build -- --config ${config} ${name}" first`);
  return bin;
}

export function busAddress(dev: { bus?: string; address?: number }): string {
  if (dev.bus === undefined || dev.address === undefined) {
    throw new Error("the usb module reported no bus/address for the flashed device; update it with npm install");
  }
  return `${dev.bus}/${dev.address}`;
}

async function waitForDetach(host: UsbHost, at: string): Promise<void> {
  const deadline = Date.now() + REENUMERATE_TIMEOUT_MS;
  while (Date.now() < deadline) {
    const present = (await host.getDevices()).some((d) => busAddress(d as UsbDevice) === at);
    if (!present) return;
    await sleep(100);
  }
  throw new Error(`the module was flashed but did not restart within ${REENUMERATE_TIMEOUT_MS / 1000}s`);
}

export async function flashImage(host: UsbHost, path: string): Promise<void> {
  const image = readFileSync(path);
  console.log(`${path}: ${image.length} bytes`);
  const dfu = await findFlashTarget(host);
  const at = busAddress(dfu.dev);
  try {
    const target = flashInterface(dfu)!;
    await dfu.claim(target);
    await dfu.toIdle();
    await dfu.write(FLASH_BASE, image);
    await dfu.toIdle();
    await dfu.verify(FLASH_BASE, image);
    console.log("verified; starting firmware");
    await dfu.leave(FLASH_BASE);
  } finally {
    dfu.close();
  }
  await waitForDetach(host, at);
}
