# Need for Speed: Hot Pursuit 2 — Native Port

Native port of the PlayStation 2 game engine for Windows x64 and Nintendo Switch.

**Game data required: PlayStation 2 PAL version 1.00 (SLES-50731).** Use the original `ZDIR.BIN` and `ZZDATA.BIN` from the disc's `NFSHP2` directory. Game data is not included.

## Nintendo Switch

### Build requirements

- devkitPro with devkitA64, libnx, and Switch tools (`elf2nro` and `nacptool`).
- CMake 3.24 or newer and GNU Make. On Windows, use the devkitPro MSYS2 shell and its CMake.
- Host Python 3 with Pillow to package the project-root `icon.png` as the Homebrew Menu icon. Set `HP2_ICON_PYTHON` at configure time if Pillow is installed in a different interpreter.
- The original Switch Mesa NVK Vulkan SDK, as used by OpenBLOPS-NX. Install the Horizon SDK headers and static libraries into `$DEVKITPRO/portlibs/switch`. The build needs `include/vulkan/vulkan_vi.h`, `lib/libvulkan.a`, and the Switch libelf, Expat, zlib, zstd, and libpng libraries.

From the repository root in the devkitPro shell:

```sh
cmake -S . -B build-nx -G "Unix Makefiles" \
  -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/Switch.cmake" \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-nx -j 8
```

### Install and run

Copy `build-nx/NFSHP2.nro` and the two PAL 1.00 archives to your SD card.

```text
sdmc:/switch/NFSHP2/
  NFSHP2.nro
  ZDIR.BIN
  ZZDATA.BIN
```

Launch through the Homebrew Menu in full application mode. The main scene renders at **1280×720** in both handheld and docked modes. New Vulkan pipelines compile in the background, and draws using an unfinished pipeline may briefly be absent on first use. The driver cache is saved on normal exit. Handheld mode requests the 460.8 MHz GPU APM profile. Docked mode uses the system profile. Saves and `NFSHP2_log.txt` are in `Saves/`.

### Frame rate and FPS counter

On first boot the port creates `config.txt` beside the running NRO. Defaults:

```ini
frame_limit=30
show_fps=0
adaptive_draw_distance=1
test_low_quality=1
language=-1
```

`frame_limit` caps gameplay presentation (including the pause scene); use `0`
for uncapped or a value from `1` to `60`. Simulation and audio keep their existing
update rates. Frontend menus retain VSync pacing. The same cap and 720p output
apply in handheld and docked mode.

**Display Options** in the frontend and **Pause > Options > Display** include
**Frame Rate** and **Show FPS** as the first two rows. Change them with left/right; leaving Display
saves both to `config.txt`. Show FPS defaults to Off. Config edits take effect
on the next launch. The menu offers 20, 24, 25, 30, 40, 45, 50, 60, and Uncapped;
manual config values between these choices are also accepted.

Choosing a language in the PAL selector saves it as a Switch-wide preference,
so loading a profile cannot switch it back to English. `language=-1` follows
the game/profile until a language is explicitly selected; `0` English, `1`
French, `2` German, `3` Italian, `4` Spanish and `5` Swedish select one directly.


This diagnostic build defaults to `test_low_quality=1`: active gameplay stays
at **0.3×** draw distance with normal scene quality and **0.5×** mirror distance.
Adaptive quality changes are suspended during the test. This also applies to existing config
files that do not yet contain the new key. Add `test_low_quality=0` to restore
the adaptive behavior below. Menus and paused frontend rendering retain normal
quality. The setting is written with the other display settings on save.

A separate no-log performance build is available at
`build-nx/pgo-use-no-logs/NFSHP2.nro` (`HP2_SWITCH_DIAGNOSTICS=OFF`). It uses the
same fixed 0.3× test profile and keeps the FPS display available. Diagnostic
files and timing summaries are disabled in that build.

Adaptive distance starts at **0.6×** and drops to **0.4×** after two consecutive
one-second windows below 28 FPS. At **23 FPS or lower**, one measured window
activates emergency scene quality with **0.2×** main distance and **0.25×** mirror
distance. This escalation can happen during an existing recovery hold.

Emergency quality uses coarser scenery and vehicle meshes, a 15/22-unit scenery
near-detail range, simpler wheels, fewer distant non-player vehicle particles,
and no car shadows, environment reflections, specular passes, decorative world
flares, skid-mark geometry or depth of field. Player/camera/munition warning
effects remain visible. AI, collisions, audio, HUD and 1280×720 output retain
their existing behavior.

After five consecutive windows at **28 FPS or better**, emergency quality ends
and distance returns to **0.4×**. Another five stable windows at **29.5 FPS or
better** restore **0.6×**. Each transition has a five-second minimum dwell before
quality increases. Thresholds scale down for caps below 30. All quality settings
restore outside active racing and after HOME resume. Set
`adaptive_draw_distance=0` to keep the normal 0.6×/0.5× quality profile.
The normal vehicle LOD multiplier remains 1.75×; emergency mode removes that
boost and simplifies non-player cars further.

### Controllers

Menus and single-player races accept input from any connected controller slot,
including handheld controls. Controllers share player one until a two-player
Challenge session is confirmed. Single Joy-Con input is mapped for horizontal
use; the Minus button on a single left Joy-Con acts as Start.

Confirming a two-player Challenge before car selection opens the libnx controller
connection screen for two players. Controller order there assigns P1 and P2.
Cancelling keeps you in race setup. Inputs remain separate through two-player
car selection and gameplay; returning to Challenge setup or the main menu
restores shared controls. Buttons held while closing the connection screen are
ignored until released.

### Custom controller artwork

Place your own **8-bit RGBA PNGs** with transparent backgrounds in
`sdmc:/switch/NFSHP2/Glyphs/`. Restart the game after changing them.

| File | Size | Replaces |
| --- | --- | --- |
| `buttons.png` | 128×256 | The complete `BUTTONSOUT` atlas used for menu and help prompts |
| `controller.png` | 256×152 | The controller picture for all four Switch gamepad layouts |
| `controller30.png` through `controller33.png` | 256×152 each | Optional per-layout pictures that take precedence over `controller.png` |

The Switch uses B to back out of menus and maps the original Square driving action (reverse in the default layout) to B in a race. The game remaps the face-button prompts for each context.

The atlas uses 26×28 cells with no spacing. Coordinates start at the
top-left. Keep each icon inside its cell and leave unused pixels transparent.
The NRO includes the project-root `buttons.png` by default. A valid SD
card `buttons.png` takes precedence over the embedded atlas.

| Top Y | X = 0 | X = 26 | X = 52 | X = 78 |
| --- | --- | --- | --- | --- |
| 0 | + | X | Right stick down | D-pad variant |
| 28 | − | B | Right stick up | Left stick click |
| 56 | Right stick left/right | ZL | Left stick down | |
| 84 | D-pad down | ZR | Left stick up | |
| 112 | D-pad up | L | Right stick click | |
| 140 | D-pad right | R | D-pad variant | |
| 168 | D-pad left | Left stick up/down | Steering wheel | |
| 196 | Y | Left stick left/right | Pedal | |
| 224 | A | Right stick up/down | Pedal | |

## Windows

### Requirements

- Windows x64 and a hardware GPU with Vulkan 1.1+ drivers.
- PAL 1.00 PS2 game data as an ISO, an archive pair, or fully extracted resources.
- Visual Studio 2022 with **Desktop development with C++**, MSVC v143 and a Windows SDK.

### Build

1. Open `vs2022/NFSHP2.sln`.
2. Select **Release | x64** and **Build Solution**.
3. Find `NFSHP2.exe`, `NFSHP2Unpack.exe` and `SDL2.dll` in `vs2022/bin/x64/Release`.

Keep `SDL2.dll` beside `NFSHP2.exe`. A separate Vulkan SDK, Clang, Python or shader compiler is not required for the Visual Studio build.

### Run

Place the executable beside the original `ZDIR.BIN` and `ZZDATA.BIN`, or select your game data explicitly:

```powershell
.\NFSHP2.exe --data "D:\Games\NFSHP2\@ZZDATA"
.\NFSHP2.exe --zzdata "D:\Games\NFSHP2\ZZDATA.BIN"
.\NFSHP2.exe --data "D:\Games\NFSHP2.iso"
```

These are alternative launch methods. The game also searches for supported data beside the executable. For extracted resources, preserve the full directory, including `__Unknown`, `__archive` and `NFSHP2_EXTRACTION_MANIFEST.json`. See [the extraction guide](tools/archive/README.md) for `NFSHP2Unpack` usage.

### Settings and controls

The pre-launch configurator provides display settings, texture filtering, MSAA, draw distance, keyboard bindings, controller selection and gamepad bindings. Use `--configure` to open it directly or `--no-configurator` to launch with saved settings.

Default keyboard controls: **W/S** accelerate/brake, **A/D** steer, **Space** handbrake, **C** camera, **Esc** pause, **Enter** confirm and **Backspace** back. Controls can be remapped in the configurator.

### Saves and troubleshooting

Profiles, settings and logs are stored in `Saves/` beside the executable. Keep this folder when updating. For bug reports, include `NFSHP2_log.txt`, any `NFSHP2_error.txt` or crash dump, and the steps to reproduce the issue.

The port is still under development. Wine/Proton audio compatibility is not verified in this base version.

Third-party notices: [SDL2](third_party/SDL2/LICENSE.txt) · [Vulkan headers](third_party/vulkan/LICENSE.md) · [Vulkan loader](third_party/vulkan/LICENSE-Loader.txt).
