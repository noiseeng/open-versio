---
name: versio-flash
description: Build a Versio firmware and write it to a module over USB with `npm run build` then `npm run flash`. Use when asked to build, compile, flash, install, update or "put on the module" a ship firmware with no debug probe attached (e.g. "build it", "flash example", "update the module", "why won't flash find my module"). To run a test or debug build and read bug_print output (over USB or a probe), use `versio-debug`.
---

# versio-flash

```sh
npm run setup                    # once; downloads gcc and OpenOCD into tools/
npm run build                    # every firmware in build.json, ship config
npm run build -- --config test   # test config: asserts on, RTT log on
npm run flash                    # writes build/ship/<name>/<name>.bin over USB DFU
npm run flash -- <name>          # needed when build.json lists more than one firmware
npm run flash -- --list          # show the DFU devices that are visible
```

## Rules

- **Run `npm run build` after every change and before every flash.** `flash` writes whatever
  `build/ship/<name>/<name>.bin` is on disk; it does not build.
- **`flash` only writes the `ship` image.** A `test` or `debug` build goes on with `npm run debug`,
  which also flashes over USB and then shows the `bug_print` log (see `versio-debug`).
- The build uses `-Wall -Wextra -Werror`: a warning is a failure. Fix it rather than adding a
  `-Wno-` flag.
- Tools come only from `tools/`, never `PATH`. If `tools/` is missing or `tools.json` changed, run
  `npm run setup`.

## What flash does

1. Finds a DFU device with internal flash at `0x08000000`: either the STM32 ROM bootloader
   (`0483:df11`) or a running firmware's "Versio Hot Swap" runtime DFU interface.
2. A running firmware is detached into the bootloader, which it waits up to 15 s for.
3. Erases, writes and verifies the image, prints `verified; starting firmware` and starts it.

## DFU mode

The ROM bootloader is entered with the two buttons on the Daisy Seed on the back of the module: hold
button 1, tap button 2, then release button 1. `images/versio-back.webp` shows where they are on each
board revision.

## Failure modes

- `the "usb" module is not installed` — run `npm install` in the repo root.
- `<bin> not found; run "npm run build -- <name>" first` — build the ship config first.
- `name a firmware; build.json lists a, b` — pass the firmware name: `npm run flash -- a`.
- `no DFU device with internal flash at 0x8000000 found` — the module isn't connected over USB, or it
  is running a firmware with no hot swap interface. Put it in DFU mode (above) and retry. Check with
  `npm run flash -- --list`.
- `no DFU bootloader ... appeared within 15s after detach` — the firmware detached but the bootloader
  never enumerated. Put it in DFU mode (above) and retry.
- `device reported dfuERROR` / `would not return to dfuIDLE` — power cycle the module and retry from
  the ROM bootloader.
- Windows, `--list` shows nothing with the module attached: the device has no WinUSB driver. Install
  WinUSB for it with Zadig (https://github.com/pbatard/libwdi/releases/download/v1.5.1/zadig-2.9.exe), once for the running firmware and once for the
  ROM bootloader the first time each shows up.
