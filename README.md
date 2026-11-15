# Need for Speed: Hot Pursuit 2 — Native PC Port

Native Windows x64 port of the PlayStation 2 PAL game engine (SLES_507.31 \ SLUS_203.62), using Vulkan graphics, XAudio2 audio and SDL2 controllers. **Original game data is required and is not included.**

## Requirements

- Windows x64 and a hardware GPU with Vulkan 1.1+ drivers.
- Complete PS2 PAL game data: an ISO, a `ZDIR.BIN` / `ZZDATA.BIN` pair, or fully extracted resources.
- For building: Visual Studio 2022 with **Desktop development with C++**, MSVC v143 and a Windows SDK.

## Build

1. Open `vs2022/NFSHP2.sln`.
2. Select **Release | x64** and **Build Solution**.
3. Find `NFSHP2.exe`, `NFSHP2Unpack.exe` and `SDL2.dll` in `vs2022/bin/x64/Release`.

Keep `SDL2.dll` beside `NFSHP2.exe`. A separate Vulkan SDK, Clang, Python or shader compiler is not required for the Visual Studio build.

## Run

Place the executable beside the original `ZDIR.BIN` and `ZZDATA.BIN`, or select your game data explicitly:

```powershell
.\NFSHP2.exe --data "D:\Games\NFSHP2\@ZZDATA"
.\NFSHP2.exe --zzdata "D:\Games\NFSHP2\ZZDATA.BIN"
.\NFSHP2.exe --data "D:\Games\NFSHP2.iso"
```

These are alternative launch methods. The game also searches for supported data beside the executable. For extracted resources, preserve the full directory, including `__Unknown`, `__archive` and `NFSHP2_EXTRACTION_MANIFEST.json`. See [the extraction guide](tools/archive/README.md) for `NFSHP2Unpack` usage.

## Settings and controls

The pre-launch configurator provides display settings, texture filtering, MSAA, draw distance, keyboard bindings, controller selection and gamepad bindings. Use `--configure` to open it directly or `--no-configurator` to launch with saved settings.

Default keyboard controls: **W/S** accelerate/brake, **A/D** steer, **Space** handbrake, **C** camera, **Esc** pause, **Enter** confirm and **Backspace** back. Controls can be remapped in the configurator.

## Saves and troubleshooting

Profiles, settings and logs are stored in `Saves/` beside the executable. Keep this folder when updating. For bug reports, include `NFSHP2_log.txt`, any `NFSHP2_error.txt` or crash dump, and the steps to reproduce the issue.

The port is still under development. Wine/Proton audio compatibility is not verified in this base version.

Third-party notices: [SDL2](third_party/SDL2/LICENSE.txt) · [Vulkan headers](third_party/vulkan/LICENSE.md) · [Vulkan loader](third_party/vulkan/LICENSE-Loader.txt).
