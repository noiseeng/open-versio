/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { spawn } from "node:child_process";
import { connect } from "node:net";
import { resolve } from "node:path";
import { exec } from "./exec.ts";
import { RTT_PORT, openocdLoadArgs, openocdRttArgs, parseRttAddress } from "./openocdArgs.ts";
import { REPO_ROOT, toolPath } from "./tools.ts";

export async function flashOpenOcd(target: string, elf: string): Promise<void> {
  const r = await exec(toolPath("openocd", "bin/openocd"), openocdLoadArgs(target, resolve(elf)), false);
  process.stdout.write(r.output);
  if (r.code !== 0)
    throw new Error(`OpenOCD exited ${r.code}; is the ST-Link attached and no debug session holding it?`);
  if (!/Verified OK/.test(r.output))
    throw new Error("OpenOCD ran but never verified the download; see its output above");
}

function connectRtt(port: number, out: (text: string) => void): Promise<void> {
  return new Promise((done, fail) => {
    let attempts = 0;
    const attempt = () => {
      const sock = connect(port, "127.0.0.1");
      sock.once("connect", () => {
        sock.setEncoding("utf8");
        sock.on("data", (text: string) => out(text));
        sock.once("close", () => done());
      });
      sock.once("error", () => {
        sock.destroy();
        if (++attempts > 60) fail(new Error(`no RTT server on port ${port} after ${attempts} attempts`));
        else setTimeout(attempt, 500);
      });
    };
    attempt();
  });
}

export async function rttOpenOcd(target: string, elf: string, out: (text: string) => void): Promise<void> {
  const gcc = toolPath("gcc", "bin/arm-none-eabi-nm");
  const nm = await exec(gcc, [elf], false);
  const address = parseRttAddress(nm.output);
  if (nm.code !== 0 || address === undefined)
    throw new Error(`no _SEGGER_RTT symbol in ${elf} (nm exited ${nm.code}); is RTT enabled in this build?`);
  process.stderr.write(`\nRTT via OpenOCD, control block ${address}, port ${RTT_PORT} (ctrl-c to stop)\n`);
  const server = spawn(toolPath("openocd", "bin/openocd"), openocdRttArgs(target, address), {
    cwd: REPO_ROOT,
    stdio: ["ignore", "inherit", "inherit"],
  });
  const exited = new Promise<void>((_, fail) =>
    server.on("close", (code) => fail(new Error(`OpenOCD RTT server exited ${code ?? 1}`))),
  );
  try {
    await Promise.race([connectRtt(RTT_PORT, out), exited]);
  } finally {
    server.kill();
  }
}
