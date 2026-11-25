---
name: versio-new-firmware
description: Start a new Versio firmware from the example by copying av_example to av_<name>, registering it in build.json and filling in the DSP the board drives (controls, per-frame audio, LEDs, buffers). Use when asked to make, start, add or write a new firmware, effect or algorithm for Versio (e.g. "make a delay", "new firmware called fold", "add a second firmware", "what does the board call on my DSP"). To build and flash it, use `versio-flash`; to run it with a probe, `versio-debug`.
---

# versio-new-firmware

## Create it

1. Copy `src/open_versio_main.cpp` to `src/frm_av_<name>.cpp` and `src/av_example.h` to `src/av_<name>.h`.
2. In both, rename `av_example` to `av_<name>` (the struct and the include).
3. Add `{ "name": "<name>", "sources": ["src/frm_av_<name>.cpp"] }` to `firmwares` in `build.json`.
4. `npm run build -- <name>`. With no name `build` builds every firmware; once `build.json` lists more
   than one, `flash` and `debug` need the name.

`src/frm_av_<name>.cpp` only runs the board with the DSP:

```cpp
int main() { return versio::versio_main<versio::av_<name><32>, 96000>(); }
```

The DSP's template argument is `FRAME_COUNT`, the stereo frames per audio block. The second argument is
the codec rate: `96000` or `48000`. Change only the two
firmware files; everything else under `src/` is board support.

## What the board uses

`versio::versio_brd<dsp_t, HARDWARE_FRAME_RATE>` (`src/lacerti/prd/versio/hardware/versio_brd.h`) reads these
from the DSP and nothing else:

- Constants:
  - `FRAME_COUNT`: frames per block.
  - `BLOCK`: `bool`. `false` calls `operator()` per frame; `true` calls
    `void block(const ctl_t&, float* l, float* r)` once per block on `FRAME_COUNT` samples per side,
    in place.
  - `color0`, `color1`, `color2`, `color3`: `numerus::color`; the boot colours of the four LEDs, faded
    into the `control` colours as audio fades in.
- `ctl_t`, constructible from `const versio_inp&`.
- `template <class alloc_t> void set_frame_rate(float fps, alloc_t* alloc)`: called once before audio
  starts.
- `versio_out control(const ctl_t&)`: once per block, before the frames. It returns the four LED colours.
- `std::array<float, 2> operator()(std::array<float, 2> in)`: once per frame, stereo in to stereo out
  (when `BLOCK` is `false`).
- Declare all three yourself (an empty body where there is nothing to do; never an empty-member base class); the board always calls them:
  - `void clear_memory()`: from the main loop after the FSU button has been held for 4 s.
  - `void print()`: from the main loop once a second in `test` and `debug`, before `cpu <n>%`.
  - `void finish()`: at the end of every block.

The example is a stereo delay: `k1` time (10 ms to 2 s), `k2` feedback, `k3` mix. It shows allocating in
`set_frame_rate` and clearing in `clear_memory`.

## Controls

`versio_inp` (`src/lacerti/prd/versio/versio_inp.h`) is what `ctl_t` is built from:

- Knobs `k1`..`k7`: 0–1, each knob plus its CV jack.
- Switches `sw1` (ABC), `sw2` (XYZ): positions 0 (bottom) to 2 (top).
- The FSU button and the FSU gate jack. The button is `btn_trig` (pressed this
  block) and `btn_gate` (held), the jack is `jack_trig` and `jack_gate`.
- Tap tempo from both: `tempo_fps` (the tapped period in frames, not BPM), `tap_cleared`,
  `last_hit`, `tap_clear_pending`.
- The meters `meter_inpl`, `meter_inpr`, `meter_outl`, `meter_outr`.

The header's comments give the units and ranges; don't guess one.

## Audio and LEDs

- Samples are `float`, nominally −1 to 1.
- `control` returns `versio_out{rgb1, rgb2, rgb3, rgb4}`, each a `numerus::color`. The example derives
  `ctl_t` from `versio_meters` and returns `vu_out(ctl)`: input and output level meters, as the panel
  labels the LEDs (In, In, Out, Out).
- `control` and `operator()` run inside the SAI DMA interrupt (half and complete transfer):
  keep them bounded and never wait in them.

## Memory

- There is no heap: no `new`, `malloc` or `std::vector`. Fixed sizes are `std::array` members.
- Buffers whose size depends on the frame rate (delay lines, loopers) come from the `alloc` passed to
  `set_frame_rate`. `alloc->alloc_array_init(count, &ptr, "name")` fills `ptr` and returns it, with every
  element zeroed or default-constructed.
  - It takes memory from the fastest zone with room (DTCM first); a buffer too big for internal RAM lands
    in the 64 MB external SDRAM.
  - It can't be freed: `free` asserts. Allocate once, in `set_frame_rate`.

## Target rules

Cortex-M7 with an FPv5 FPU (`-mfpu=fpv5-d16`), `-ffast-math`, no exceptions, no RTTI, `-Wall -Wextra
-Werror`:

- Use `float`. `-Wdouble-promotion` with `-Werror` makes an implicit promotion to `double` a build
  error, and `-fsingle-precision-constant` makes unsuffixed literals `float`.
- `<cmath>` is available (newlib's libm): `std::exp2`, `std::sin`, `std::floor` and the rest.
- Don't rely on NaN or infinity: `-ffast-math` assumes they never occur. Clamp instead.
