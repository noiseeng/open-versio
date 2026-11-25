# Agent rules for open-versio

## Commands

- `npm run setup` once; `npm run build` after every change. The build uses `-Wall -Wextra -Werror`, so a
  warning is a failure.
- `npm run flash [-- <name>]` writes `build/ship/<name>/<name>.bin` over USB DFU. The name may be omitted
  while `build.json` lists one firmware.
- `npm run debug [-- <name>] [test|debug|ship] [--probe usb|jlink|stlink]` builds (default `test`),
  loads it and prints its RTT log; `bug_print` output appears there. The default `usb` needs no
  probe: it flashes over USB DFU and reads the log over USB.
- `.claude/skills/versio-flash` and `.claude/skills/versio-debug` cover these commands, their output
  and their failure modes; read the matching one before building, flashing or debugging.
- `.claude/skills/versio-crash` covers a crash, lockup or reboot loop: read the `hardfault`/`bt` report
  in the log, or attach with a probe without resetting when the core locked up. Read it before touching
  a module that has stopped.
- `.claude/skills/versio-new-firmware` covers starting a firmware from the example and what the board
  calls on the DSP. Read it before writing a new firmware.

## Writing a firmware

- A firmware is `src/frm_av_<name>.cpp`, whose `main` returns
  `versio::versio_main<dsp_t, 96000>()`, and the DSP header `src/av_<name>.h`. Add
  `{ "name": "<name>", "sources": ["src/frm_av_<name>.cpp"] }` to `build.json`'s `firmwares`.
- Copy `src/open_versio_main.cpp` and `src/av_example.h` as the starting point. Change only the firmware
  files; everything under `src/lacerti/` and the other `src/` directories is board support.
- The target is a Cortex-M7 with an FPv5 FPU, built with `-ffast-math`, `-Wdouble-promotion`,
  `-fno-exceptions` and `-fno-rtti`: use `float`, no `throw`, no heap allocation, no NaN or infinity.

## The `dsp_t` contract

`versio_brd<dsp_t, HARDWARE_FRAME_RATE>` runs the DSP once per block of `FRAME_COUNT` stereo frames:
`control(ctl)`, then `operator()` per frame (or `block(ctl, l, r)` once when `BLOCK` is true).

- Constants: `FRAME_COUNT`, `BLOCK` (`bool`), `color0`..`color3` (`numerus::color`, the boot colours).
- `HARDWARE_FRAME_RATE` (the `versio_main` argument): the codec rate, `96000` or `48000`.
- `ctl_t`: constructible from `versio_inp` (`src/lacerti/prd/versio/versio_inp.h`): knobs `k1`..`k7`
  (0–1, knob plus its CV), switches `sw1` `sw2` (0–2), the FSU button as `btn_trig`/`btn_gate` and the
  FSU jack as `jack_trig`/`jack_gate`, tap tempo `tempo_fps` (a period in frames),
  and the meters.
- `void set_frame_rate(float fps, alloc_t*)`: called before audio starts; the only place to allocate.
- `versio_out control(const ctl_t&)`: per block; returns the four LED colours.
- `std::array<float, 2> operator()(std::array<float, 2> in)`: per frame.
- Declare `clear_memory()` (main loop, after the FSU button is held 4 s), `print()` (main loop, once a
  second before the `cpu <n>%` line) and `finish()` (end of each block) yourself, with an empty body
  where the DSP has nothing to do; the board calls them unconditionally. Never put them in an
  empty-member base class.
