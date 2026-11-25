Detailed Usage
===

## Setup

```sh
npm i
npm run setup
```

`npm i` installs the build scripts npm dependencies.
`npm run setup` downloads gcc 13.3.1 and OpenOCD for your OS into `tools/`.

## Build

```sh
npm run build
npm run build -- --config test
npm run build -- --config debug example
```

With no name every firmware in `build.json` is built. There are three configs:

* `ship` (default): optimized, no asserts, no logging.
* `test`: optimized, with asserts and RTT (log printing).
* `debug`: optimized, with asserts, RTT and the `DEBUG` defines.

The image is `build/<config>/<name>/<name>.bin`, with the `.elf` and `.map`
next to it.

## Flash over USB

```sh
npm run flash
npm run flash -- --list
```

`flash` writes the `ship` image over USB DFU and verifies it. If the module is
running a firmware built from this repository it is switched into its
bootloader for you (through the "Versio Hot Swap" interface). Otherwise it uses
the STM32 ROM bootloader (`0483:df11`). If nothing is found put the module in
DFU mode: hold button 1, tap button 2, then release button 1 (see the
[README](README.md) for where they are).
`--list` shows what DFU devices are visible to help troubleshoot.

With more than one firmware in `build.json` name the one you want:
`npm run flash -- <name>`.

On windows the DFU device needs the WinUSB driver. If `--list` shows nothing
with the module attached run [zadig](https://github.com/pbatard/libwdi/releases/download/v1.5.1/zadig-2.9.exe), select the module
(and again for the bootloader device the first time it shows up) and install
WinUSB as the driver for the module.

## Debug and print

```sh
npm run debug
npm run debug -- test --probe stlink
npm run debug -- example debug --no-build
```

`debug` builds the `test` config (or the one you name), loads it and then
streams the RTT log, which is where `bug_print` output goes. Lines you type are
sent to the firmware's RTT input. Ctrl-c stops it.

By default this all happens over the USB cable: no JTAG probe, no soldered
header. It flashes over USB DFU like `flash`, then reads the log from a second
USB interface that `test` and `debug` firmwares add, and reconnects by itself
if the module restarts. Use this instead of a probe for printing, measuring
and listening.

A crash or failed assert prints a `hardfault` line and a `bt` (backtrace) line,
and `debug` prints the source file and line of each address under them. The
module then mutes its audio, turns all four LEDs magenta and keeps the USB log
and flashing working, so fix the code and run `npm run debug` again without
touching the module. A probe is still needed to step through code, and for the
rare crash that stops the USB log entirely (a stack overflow, or a crash in the
USB code itself).

Options:

* `--probe usb` (the default) needs only the USB cable.
* `--probe jlink` uses a SEGGER J-Link. J-Link software can't be
  downloaded by `setup`; install it from SEGGER to its default location.
  `npm run find-jlink` shows which install will be used.
* `--probe stlink` uses an ST-Link through the OpenOCD `setup` downloaded.
* `--no-build` loads what is already built.
* `--no-rtt` loads and exits. `ship` builds have no RTT, so this is implied.

Disconnect the USB cable while reading the log with a probe: the firmware can't
tell that a probe is attached, so with both connected part of the log goes to
USB and never reaches the probe.

It doesn't start a debugger. To step through code attach your debugger of choice
to the probe and load `build/debug/<name>/<name>.elf` for symbols.

`test` and `debug` firmwares print `cpu <n>%`, the share of the available time
your DSP uses, once a second.

## Test mode

Hold the FSU button down while the module powers up to enter a hardware test
mode: your DSP does not run, audio passes from input to output and the LEDs show
the controls. Power cycle to leave it. If your firmware seems to do nothing,
check you weren't holding the button.

## Making your own firmware

Start from the example: copy `src/open_versio_main.cpp` and `src/av_example.h` to
`src/frm_av_<name>.cpp` and `src/av_<name>.h` and add
`{ "name": "<name>", "sources": ["src/frm_av_<name>.cpp"] }` to `firmwares` in
`build.json`.

There is no general purpose heap. The hardware interface passes in an allocation
object that can provide buffers at init time from the various memory zones on
the chip. There is no free.
