---
name: versio-crash
description: Find out why a Versio firmware crashed, locked up or keeps rebooting, from the `hardfault`/`bt` report a test or debug build prints over USB (with source lines from `npm run debug`), by narrowing it down with bug_print over USB, or by attaching a J-Link or ST-Link without resetting the chip when the core locked up, reading the Cortex-M7 fault-status registers and the stacked exception frame, and mapping PC/LR to source lines. Use when the module freezes, goes silent, reboots or boot-loops, the RTT log stops, or a `bug_assert` line appears (e.g. "it crashed", "it locked up", "why did it hard fault", "it keeps restarting"). To load and run a firmware, use `versio-debug`.
---

# versio-crash

## What a crash looks like

- `test` and `debug`: every fault, and every failed `lac_assert` / `bug_assert`, ends in
  `irq_hardfault` (`src/lacerti/mcu/stm/h750/mcu_h750.h`). It prints two lines to the log:
  - `hardfault pc <pc> lr <lr> psr <xpsr> sp <frame> exc <exc_return> hfsr <hfsr> cfsr <cfsr> mmfar <mmfar> bfar <bfar>`
  - `bt <addr> ... | <addr> ...`: return addresses found on the stack, innermost first. `|` marks an
    interrupt: the address after it is where the interrupted code was. These are candidates read from
    the stack, so an entry can be stale; the first few are reliable.
- `npm run debug` adds a `crash: <cause>` line decoded from `cfsr`/`hfsr` (e.g. `bus fault accessing
  0x60000000`), what the module is doing now, then the call chain innermost first as
  `function  path:line`, using `addr2line` from `tools/gcc-<version>/bin/` and the elf it just built.
  Template arguments and parameters are collapsed to `<…>` and `(…)`; inlined calls are listed as
  their own frames.
- With no probe attached the firmware then keeps running a post-mortem loop: audio is muted, all four LEDs
  turn magenta, and the USB log and `npm run debug` / `npm run flash` keep working, so read the report,
  fix the code and reflash without touching the module.
- Asserts are slim (`BUG_ASSERT_SLIM=1` in `build.json`): a failed assert prints no text of its own.
  It shows as `cfsr 10000` (UNDEFINSTR) with `pc` in `bug::abort`. `test` and `debug` are both
  optimized, so GCC merges every assert of one function into a single `udf`: the pc names that
  function but not which of its asserts failed. Add a `bug_print` before each suspect assert to tell
  them apart.
- An interrupt with no handler goes to `irq_unused`, which prints `Unused IRQ <n>` first.
- The core locks up instead, with the report still in the RTT buffer, when a debugger has been attached
  since power-on, when the fault is in the USB interrupt, or when it happened while stacking the
  exception (a stack overflow). A probe reads it; without one, narrow it down with prints.
- USB starts before `set_frame_rate`, so a crash there is reported too. A crash in the DSP's constructor
  comes before USB and locks up, with no USB log to read it from.
- `ship`: no report; `irq_hardfault` calls `reset_handler()`, so a crash restarts the firmware. One crash
  looks like a reboot, and a repeating one like a boot loop. Reproduce it with a `test` build
  (`npm run debug`).

## Without a probe

### Get the call stack

Start the log before reproducing the crash and leave it running. It never exits, so an agent runs it in
the background with its output in a file:

```sh
mkdir -p tmp
npm run debug > tmp/debug.log 2>&1
```

1. Wait for `USB log ... (ctrl-c to stop)` in the file, then reproduce the crash, asking the user to turn
   knobs or patch cables if it takes that.
2. Read from the `crash:` line down: innermost call first. The first frame in the firmware's own source is
   the place to start. The frame marked `| interrupted:` is what the audio interrupt broke into, usually the
   main loop, and is rarely the cause.
3. `pc` in `bug::abort` is a failed assert: the next frames name the function that asserted.
4. `bus fault on a buffered write` has a late pc: the bad write is a little earlier. `bus fault accessing
   <addr>` or `memory access violation at <addr>` gives the bad address; look it up in the `.map`.
5. A stack that differs on every run suggests memory was overwritten earlier: check array indexes and
   large locals (the stack is 4 KB).

If the LEDs are already magenta and no log was running, `npm run debug` would reflash and lose the report.
Read it from the module's buffer instead:

```sh
node --input-type=module -e "import { loadUsb } from './scripts/usbFlash.ts'; import { streamLog } from './scripts/usbLog.ts'; await streamLog(await loadUsb());"
```

This prints the raw `hardfault` and `bt` lines; decode them with "Map to source" before rebuilding the elf.
It works only for a crash within the first half minute or so with no reader: after that the once-a-second
`cpu` lines have filled the buffer and the report was dropped. Then reproduce it with the log running.

### Find the cause with prints

Print the values the crashing line uses, run `npm run debug` again and reproduce:

```cpp
bug_print("pos %d gain %.3f\n", (int)pos, (double)gain);
```

- Formats: `%d %i %u %x %p %c %s %f`. Cast a `float` to `double`: the build has `-Wdouble-promotion -Werror`.
  `%f` prints `nan` and `inf`.
- **Print rarely.** The log buffer is 1 KB and drops whatever does not fit, including the crash report.
  `control()` runs once per `FRAME_COUNT` frames (3000 times a second in the example) and `operator()` once
  per frame, so print on a change or on a counter:

```cpp
if(++print_count >= 1500)
{
	print_count = 0;
	bug_print("gain %.3f\n", (double)gain);
}
```

- **Stop at the first bad value**, not where it finally crashes. Print it and call `bug::abort()`; the
  report then shows the calls that led there:

```cpp
if(index >= buffer.size())
{
	bug_print("index %u phase %.3f\n", (unsigned)index, (double)phase);
	bug::abort();
}
```

- The build uses `-ffast-math`, so `x != x` and `std::isnan` are unreliable. Test the bits (`#include <bit>`):
  `(std::bit_cast<uint32_t>(x) & 0x7f800000u) == 0x7f800000u` is NaN or infinity. A NaN does not fault by
  itself; it does its damage later, e.g. when converted to an index.
- A print can move or hide a timing or memory-overwrite bug; if it does, suspect one of those.
- Remove the prints once the cause is fixed.

### When there is no report

- **The log stops and the LEDs are not magenta.** The main loop sends the log, so it stops when the main loop
  hangs, when the audio callback never returns or uses all the CPU, and on a lockup. If the last
  `cpu ... max ... overruns ...` lines were climbing, it is overload. Cut the work (return early from
  `operator()`) until the log comes back, then restore it in halves.
- **Markers.** Print once at each stage of the suspect path. The last line to arrive is a lower bound: later
  ones may still have been in the buffer.
- **A crash before USB starts** (the DSP's constructor). Prints cannot get out. Move the work into
  `set_frame_rate`, where a crash is reported.
- **Reboots.** `test` and `debug` do not restart on a fault. If the log closes and reconnects, suspect power,
  the cable or a `ship` build.
- **It will not reflash.** Put the module in its bootloader with the Daisy Seed buttons (`versio-flash`), then
  run `npm run debug`.

## With a probe

Never flash, run `npm run debug` or reset before reading the fault: each one wipes the state. Stop any
running `npm run debug` (it holds the probe), then attach to the chip as it is. The `hardfault` and `bt`
lines are in the RTT buffer: a J-Link or ST-Link RTT session prints them, and they hold the same data as
the registers below.

## Attach without reset

J-Link (`npm run find-jlink` prints `<jlink-dir>`):

```sh
<jlink-dir>/JLinkExe -device STM32H750IB -if SWD -speed 4000 -autoconnect 1
```

Then, at the `J-Link>` prompt (never `r` or `loadfile`):

```
h
regs
mem32 0xE000ED04 1
mem32 0xE000ED28 4
mem32 0xE000ED38 1
mem32 <frame> 8
q
```

ST-Link, with the OpenOCD in `tools/openocd-<version>/` (version in `tools.json`):

```sh
tools/openocd-<version>/bin/openocd -f interface/stlink.cfg -f target/stm32h7x.cfg -c init -c halt -c reg -c "mdw 0xE000ED04 1" -c "mdw 0xE000ED28 4" -c "mdw 0xE000ED38 1" -c "mdw <frame> 8" -c shutdown
```

`<frame>` is the `sp` value in the `hardfault` line. Without that line, the frame is above
`hardfault_report`'s own stack frame (and, in `debug`, above `bug::abort`'s too): SP at the halt plus the
bytes their `push` and `sub sp` take in the disassembly.

## Reading it

| address | register | meaning |
|---|---|---|
| `0xE000ED04` | ICSR | bits 8:0 = active exception; 3 is HardFault |
| `0xE000ED28` | CFSR | the cause; decode below |
| `0xE000ED2C` | HFSR | bit 30 FORCED: a configurable fault escalated to HardFault; bit 1 VECTTBL |
| `0xE000ED30` | DFSR | debug events; ignore |
| `0xE000ED34` | MMFAR | faulting address if CFSR bit 7 MMARVALID |
| `0xE000ED38` | BFAR | faulting address if CFSR bit 15 BFARVALID |

CFSR bits:

- MemManage, 7:0: 0 IACCVIOL (executed from a no-execute region), 1 DACCVIOL (data access the MPU
  forbids), 7 MMARVALID.
- BusFault, 15:8: 8 IBUSERR, 9 PRECISERR (bad data address, exact PC), 10 IMPRECISERR (bad write, the PC
  is past it), 15 BFARVALID.
- UsageFault, 31:16: 16 UNDEFINSTR, 17 INVSTATE, 18 INVPC, 19 NOCP, 24 UNALIGNED, 25 DIVBYZERO.

UNDEFINSTR with the stacked PC on a `udf` is an assert or `bug::abort()`, not a corrupt instruction.

The stacked frame is `r0 r1 r2 r3 r12 lr pc xpsr`, eight words at `<frame>`. `exc` bit 4 clear means
an FPU frame (26 words); bit 2 set means it is on PSP.

## Map to source

`npm run debug` already does this for the `hardfault` and `bt` lines. By hand, use the elf of the build
that is on the chip; for a return address (`lr`, `bt`) look up two bytes before it to land on the call:

```sh
tools/gcc-<version>/bin/arm-none-eabi-addr2line -f -C -i -e build/test/<name>/<name>.elf <pc> <lr>
tools/gcc-<version>/bin/arm-none-eabi-objdump -d -C build/test/<name>/<name>.elf > /tmp/<name>.dis
```

`build/<config>/<name>/<name>.map` gives each symbol's address and size, which helps with a BFAR or
MMFAR value. `test` and `debug` builds are both optimized, so addr2line may point at the inlined
callee; `-i` lists the inline chain.
