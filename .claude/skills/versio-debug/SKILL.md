---
name: versio-debug
description: Build a Versio firmware, load it and stream its RTT log with `npm run debug` — over USB by default (no probe needed), or through a J-Link or ST-Link debug probe. Use when asked to run, debug or test a firmware on hardware, or to see bug_print output (e.g. "run it on the module", "show me the log", "the J-Link won't connect"). To write a ship build with no log, use `versio-flash`; for a crash, lockup or reboot loop, use `versio-crash`.
---

# versio-debug

```sh
npm run debug                              # build test, flash over USB, stream the RTT log over USB
npm run debug -- <name> [test|debug|ship]  # name needed when build.json lists several firmwares
npm run debug -- --probe jlink             # load and stream through a SEGGER J-Link
npm run debug -- --probe stlink            # ST-Link through the OpenOCD in tools/
npm run debug -- --no-build                # load what is already built
npm run debug -- --no-rtt                  # load and exit
npm run find-jlink                         # show which J-Link install is used
```

The config defaults to `test`. It builds `build/<config>/<name>/<name>.elf` (and `.bin`), loads it,
resets and runs it, then streams RTT channel 0 to stdout until ctrl-c. Typed lines go to RTT down
channel 0.

## Configs

- `test`: optimized, asserts and RTT on. Use it for anything you measure or listen to.
- `debug`: like `test` plus the `DEBUG` defines; also optimized, so a debugger shows some locals as
  optimized out.
- The firmware prints `cpu <n>%` (DSP load) once a second in `test` and `debug`.
- `ship`: what `npm run flash` writes. No RTT and no asserts, so nothing is streamed.

## Reading the log

- Every `bug_print(...)` in the firmware appears in the RTT stream. `bug_print` compiles to nothing in
  `ship`.
- A crash or failed assert in `test` or `debug` prints a `hardfault ...` line and a `bt ...` line, and
  `npm run debug` prints the source line of each address under them. The firmware then mutes, turns all four
  LEDs magenta and keeps the USB log and DFU alive, so reflash with `npm run debug` as usual.
  `versio-crash` reads the report.
- A USB log that goes silent and stays silent is a crash the firmware could not report over USB (a
  fault in the USB interrupt or a stack overflow) or a hang; `versio-crash` covers both, with or without a
  probe.

## Probes

- **USB** (default): flashes `build/<config>/<name>/<name>.bin` over the module's USB DFU like
  `npm run flash`, waits for it to restart, then reads the RTT log from a second USB interface that
  `test` and `debug` firmwares add. Needs only the USB cable. If the module restarts or is unplugged,
  the log reconnects by itself.
- **J-Link** (`--probe jlink`): the SEGGER J-Link software isn't downloadable by `setup`. Install it
  from https://www.segger.com/downloads/jlink/ to its default location; the scripts search only the
  SEGGER install directories. `npm run find-jlink` prints what was found and where it searched.
- **ST-Link** (`--probe stlink`): uses `tools/openocd`, no install needed. RTT is found through the
  `_SEGGER_RTT` symbol in the elf and served on port 9090.
- A J-Link or ST-Link connects over SWD to the STM32H750IB. Only one program can hold the probe:
  close any IDE debug session first.
- `debug` does not start a debugger. To step through code, attach your own debugger to the probe and
  load `build/debug/<name>/<name>.elf` for symbols.

## USB and a probe together

- **Disconnect USB while reading the log with a probe.** The firmware can't tell that a probe is
  attached, so part of the log would go to USB and never reach the probe.

## Failure modes

- `waiting for a module running a debug or test firmware over USB...` — no module with the USB log
  is connected. Check the cable, and that the firmware is a `test` or `debug` build.
- `no output yet; ...` — nothing has been printed yet, or a probe is taking the log.
- `no DFU device with internal flash at 0x8000000 found` — see `versio-flash`.
- `no J-Link install found; searched: ...` — install J-Link to one of the searched directories.
- `J-Link exited <n>; is the probe attached and no debug session holding it?` /
  `OpenOCD exited <n>; is the ST-Link attached ...` — probe unplugged, module unpowered, or another
  program holds the probe.
- `J-Link ran but never confirmed the download` / `OpenOCD ran but never verified the download` —
  the probe connected but the load failed. Read the tool output printed above it.
- `<elf> not found; run without --no-build` / `<bin> not found` — nothing built for that name and
  config yet.
- `no _SEGGER_RTT symbol in <elf>` — a `ship` build has no RTT; use `test` or `debug`.
- `no RTT server on port 9090 after N attempts` — OpenOCD never found the RTT block. Check the firmware
  runs (it may have hit an assert before printing) and retry.
- RTT connects but prints nothing — the firmware hung before its first print. Build `debug`, attach a
  debugger and halt to see where it is.
- `gcc <version> is not installed: run npm run setup` before the log starts — `tools/` has no gcc,
  which also supplies the `addr2line` the crash report uses. Run `npm run setup`.
- `addr2line failed: ...` under a `hardfault` line — `addr2line` ran but failed (e.g. the elf was
  rebuilt or deleted since flashing). The raw `hardfault` and `bt` addresses above it are still valid.
