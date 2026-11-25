/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

import { spawn } from "node:child_process";
import { REPO_ROOT } from "./tools.ts";

export interface RunResult {
  code: number;
  output: string;
}

export function exec(exe: string, args: string[], inherit: boolean): Promise<RunResult> {
  return new Promise((resolveRun) => {
    const child = spawn(exe, args, { cwd: REPO_ROOT, stdio: inherit ? "inherit" : ["ignore", "pipe", "pipe"] });
    let output = "";
    child.stdout?.on("data", (d) => (output += String(d)));
    child.stderr?.on("data", (d) => (output += String(d)));
    child.on("error", (e) => resolveRun({ code: 1, output: `${exe}: ${e.message} (cwd ${REPO_ROOT})\n` }));
    child.on("close", (code) => resolveRun({ code: code ?? 1, output }));
  });
}
