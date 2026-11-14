# Need for Speed Hot Pursuit 2 — Native Windows port

A Windows x64 port of the PlayStation 2 version of Need for Speed Hot Pursuit 2. The engine uses hardware Vulkan graphics, XAudio2 audio, Windows keyboard input and SDL2 controllers. The source targets the original PAL game and its resources. Game assets are supplied separately.

The project is under development. Rendering performance in scenes with many visible cars remains an open issue. The solution builds the game and `NFSHP2Unpack`, a standalone resource extraction and verification utility.

## Build

1. Install Visual Studio 2022 with **Desktop development with C++**, the **v143** toolset and a Windows SDK.
2. Extract this source package into a new directory.
3. Open `vs2022/NFSHP2.sln`.
4. Select **Release** and **x64**, then choose **Build → Build Solution**.
5. Use the files in `vs2022/bin/x64/Release`.

The application files are `NFSHP2.exe`, `NFSHP2Unpack.exe`, `SDL2.dll` and `SDL2-LICENSE.txt`. Keep `SDL2.dll` beside the game executable. The solution also produces the engine and core static libraries as build outputs.

The source uses C11 and C++17. The Release game targets use **v143**, the static runtime **/MT**, **/W4 /WX** and **/fp:precise**. The archive utility also uses **/MT /W4 /WX** and retains its own strict floating-point setting.

Vulkan headers and the Windows import library are included. The computer running the game needs a hardware graphics device with a Vulkan 1.1 or newer driver. Normal startup rejects CPU Vulkan devices and reports an error when no suitable hardware device is available. Software Vulkan is available only through an explicit maintainer diagnostic option. Shader binaries are included in the source project; the normal Visual Studio build does not require Python, Clang, a Vulkan SDK installation or a shader compiler.

An optional build with CMake 3.24 or newer is also available. Run these commands from the project directory:

```text
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target NFSHP2 NFSHP2Unpack
```

## Game data and first launch

Use data from the **PlayStation 2 PAL game**: a complete `ZDIR.BIN` / `ZZDATA.BIN` pair, a complete extraction of that archive, or an ISO9660 disc image with 2048-byte sectors. The resource layout expected by this port is the PS2 layout.

For an archive installation, put `NFSHP2.exe` and `SDL2.dll` beside both archive files. To select the archive explicitly:

```text
.\NFSHP2.exe --zzdata "D:\Games\NFSHP2\ZZDATA.BIN"
```

For an extracted installation, create a new output directory and verify it:

```text
.\NFSHP2Unpack.exe --input "D:\Original\NFSHP2\ZZDATA.BIN" --output "D:\Games\NFSHP2\@ZZDATA"
.\NFSHP2Unpack.exe --input "D:\Original\NFSHP2\ZZDATA.BIN" --verify "D:\Games\NFSHP2\@ZZDATA"
.\NFSHP2.exe --data "D:\Games\NFSHP2\@ZZDATA"
```

Keep the entire extraction, including `__Unknown`, `__archive`, `PAL.TAG` when present and `NFSHP2_EXTRACTION_MANIFEST.json`. Resources without a known filename are loaded by their archive hash. The original `ZZDATA.BIN` is no longer needed at launch when `--data` selects a complete extraction. See [the archive utility guide](tools/archive/README.md) for extraction, verification and individual resource export.

For a disc image:

```text
.\NFSHP2.exe --data "D:\Original\NFSHP2.iso"
```

Without a data argument, the game searches beside its executable and in the installation's `NFSHP2`, `@ZZDATA`, `ZZDATA` and `Data` subdirectories, including `NFSHP2\@ZZDATA` and `NFSHP2\ZZDATA`. It searches for an extracted resource directory before falling back to an archive pair. Keep an extraction separate from the original archive pair; selecting a directory that contains both uses its archive pair. Use `--data` or `--zzdata` to select the intended source explicitly.

## Settings

The native settings window opens before the game. **Save** stores the launcher choices; **Play** saves them and starts the game. Use `--configure` to open settings without requiring game data, or `--no-configurator` to start directly with saved settings. In-game language, audio levels and career saves are managed by the game's own options and save screens.

| Page | Settings |
|---|---|
| Display | Monitor, render resolution, borderless fullscreen, VSync, render FPS cap or unlock, independent FPS counter, intro movies |
| Graphics | Original texture settings or bilinear/trilinear/anisotropic filtering, MSAA, world draw distance and rear mirror distance |
| Players | Independent input mode and controller selection for both players, automatic device assignment and vibration |
| Keyboard | Separate key bindings for Player 1 and Player 2 |
| Gamepad | Button, trigger, stick and hat bindings; axis inversion, deadzone, saturation and vibration strength |
| Advanced Settings | Debug tools, content unlock, racer nitro, playable traffic, Catch Up override and supported cockpit views |

## Default Player 1 keyboard controls

| Action | Key |
|---|---|
| Accelerate / brake | W / S |
| Steer | A / D |
| Menu navigation | Arrow keys |
| Confirm / back | Enter / Backspace |
| Pause | Esc |
| Handbrake | Space |
| Change camera / alternate back | C |
| Look back | Left Shift |
| Shift down / up or context action | Q / E |
| Horn | H |
| Reset car / Select | Tab |
| Nitro / right stick click | N |

These are the native keyboard defaults. Change bindings on the **Keyboard** page; the in-game controller configuration also determines how logical buttons are used. On that page, Esc cancels key capture and Delete clears a binding. Use **Bind Esc** or **Bind Delete** to assign those keys themselves.

## Saves and diagnostics

By default, saves, settings and logs are stored in `Saves` beside the executable. **Advanced Settings → Open folder** opens the active profile directory. Use `--profile <directory>` to select another directory.

| File | Purpose |
|---|---|
| `profile.json` | Current game profile, earned progress and in-game preferences |
| `profile.backup.json` | Previous valid profile retained when saving |
| `NFSHP2_pc.ini` | Launcher display, input, graphics and advanced settings |
| `config.ini` | Boot track-selection configuration |

A new profile enables autosave during the normal first-launch flow. An existing profile keeps its saved autosave choice. The launcher's **Save** button writes `NFSHP2_pc.ini`; use the game's save/autosave controls for career progress. `--config <file>` selects the boot track configuration, while launcher settings continue to use `NFSHP2_pc.ini` in the active profile directory.

Preserve the entire profile directory when updating the executable. The loader reports an invalid existing profile and retains it; it does not replace it with an empty profile. A valid backup can be recovered automatically when the primary profile is absent.

New native replay recordings use format 11 to identify the corrected steering behavior. Earlier PAL and native recordings keep their recorded timestep and original steering law. Older engine builds reject format 11 instead of playing it with different steering behavior.

The session log is `NFSHP2_log.txt`; previous sessions may be retained as `NFSHP2_log.previous.txt` and `NFSHP2_log.previous2.txt`. A startup or runtime failure may also produce `NFSHP2_error.txt` and, for a captured native fault, `NFSHP2_crash.txt` / `NFSHP2_crash.dmp`. Include these files, selected graphics and input settings, track, car, event and reproduction steps in an issue report. For build errors, include the full **Build Output**, including the warning immediately before a warnings-as-errors failure.

The log records `render.timing_window` about every five seconds. `cpu_update_ms` includes game update, scene capture and sealing; `cpu_capture_ms` and `cpu_seal_ms` identify the latter two parts, while `cpu_update_only_ms` is the remaining update and frame-management time. It includes input, simulation, cameras and audio, not just physics. `cpu_gather_ms` measures renderer preparation and `cpu_submit_ms` covers recording, submission and other renderer work. GPU time comes from a separately identified completed submission, so it must not be added to CPU time as the frame duration.

`logical_updates`, `scene_captures`, `prepared_frames`, `frames` and `window_ms` distinguish simulation work, new scene captures and actual rendering with reused preparation. `renderer.work` also reports the available CPU budget, the distinct threads that actually recorded commands, the recording chunks, the native draw-call count and the number of preflight resident-geometry validation jobs. `secondary_reused` counts retained secondary command buffers actually executed in the current submission; those buffers do not count as newly recorded chunks. The draw-call count still includes their executed draws. These are work measurements; they do not report total CPU or GPU utilization.

Run `NFSHP2.exe --help` to open the command-line reference. For example, this starts a selected extraction with saved settings overridden to a windowed 1280 × 720 display:

```text
.\NFSHP2.exe --data "D:\Games\NFSHP2\@ZZDATA" --no-configurator --windowed --width 1280 --height 720
```

## Debug controls

Debug tools are optional and disabled by default. Enable **Advanced Settings → Enable debug menu and debug cameras**. In an ordinary single-player race, hold **Start / Menu**, then press **Select / Back / View** to give the first player's input to the debug camera. With the default Player 1 keyboard bindings, hold **Esc**, then press **Tab**. Release all buttons, triggers and sticks before using the camera. The same combination returns to driving and restores the previous view.

Short **Select** cycles the available camera modes. The simulation continues while camera control is active. Camera focus is unavailable in the frontend, pause menu and split-screen gameplay. The second player's input remains independent. See [debug controls](docs/DEBUG_CONTROLS.md) for the camera bindings and the existing menu actions.

## Rendering and performance

The default Vulkan renderer evaluates solid vertex lighting, specular response, reflection direction and environment-map coordinates on the GPU, together with position transforms, rasterization, texture sampling and compositing. Each draw supplies immutable light, material, camera and alpha inputs. The vertex shader uses the decoded frontend or in-game material coefficients and their original specular response tables, retaining the source accumulation, alpha and color-quantization rules. The CPU does not shade or generate reflection coordinates for the Vulkan solid path.

Resident geometry contains source positions, normals, texture coordinates and authored colors. Base, specular and environment passes can reuse that geometry with their own material inputs. A camera or light change updates draw data without baking new colors into the vertex cache; actual position, normal, texture-coordinate, color or topology changes invalidate the affected source geometry. This includes animated geometry. Cache capacity misses still use the same GPU lighting and reflection path.

Repeated light and material inputs share an exact per-frame shading record. The cache compares the consumed values, including the camera and material coefficients; hash collisions are checked against the complete record bytes. Fresh scene preparation performs exact source and topology validation for its resident geometry; admitted unchanged presentations reuse that preparation. For a sufficiently large warm scene, independent source comparisons use the shared CPU pool; the render owner applies their results at the original first use, retains invalidation and eviction checks, and publishes the commands in order.

Independent model-strip preparation, skinning, culling and scenery work use a shared native CPU worker pool when there is enough work. The worker budget follows the processors available under process/thread affinity and the number of useful jobs, with no fixed application-wide worker cap. Small workloads and a single available processor stay on the calling thread. Workers write disjoint results and finish before the render owner publishes them. This scheduling does not change the simulation timestep, the number of active cars, authored materials or the selected graphics settings.

Work is admitted at model, geometry and scenery batch boundaries. A scene with many small models can still spend substantial time in serial capture; the number of visible objects alone does not make every stage parallel.

For substantial draw runs, CPU workers also record secondary Vulkan command buffers from immutable draw state, using separate command pools. The render owner retains resource creation and retirement, cache mutation, draw ordering, render-target dependencies and submission, and executes the completed command buffers in source order. Scene owners may prepare source data while a preceding GPU submission runs, but the native backend still waits for its single real Vulkan fence before it reuses per-frame GPU allocations or records the next submission. It does not support multiple independently buffered GPU submissions in flight. Existing fence and image dependencies protect every retained buffer, descriptor and attachment. Remaining ordered CPU work can still limit a dense scene, so this division of work does not guarantee a particular frame rate.

During a race, additional presentations between logical updates can reuse the backend's owned scene preparation when its source inputs remain unchanged. Each such presentation constructs a new primary command buffer and submits the complete GPU render graph. Substantial, unchanged draw runs may execute their completed secondary command buffers again **for that exact prepared scene** after the preceding GPU fence completes. Resource retirement, descriptor updates and per-frame allocation lifetimes remain protected by the real fence. This deliberately does not cache individual command chunks across independently prepared logical scenes: the extra comparisons and retention were slower when most draw states changed in hardware-heavy scenes.

A new logical update, resource change, changed overlay text, render extent or relevant sun-depth result requires a fresh scene preparation. The normal render still computes the same geometry, shadows, materials and reflection passes, and draws the complete GPU frame. There is one GPU submission in flight; an independent multiple-frames-in-flight pipeline and a broad reduction in the number of GPU draw calls have not yet been validated. Performance comparisons must use the same race, car population, camera, graphics options and frame sequence.

The cockpit option in Advanced Settings controls availability in camera switching and dashboard resource admission for supported single-player cars. With the option off, the interior view is excluded even if Debug tools permits cycling through all camera kinds. The car-selection Help overlay hides all authored and optional Generic traffic thumbnails while it is open and during closing, restoring the list afterwards. Selecting a playable Generic traffic vehicle also hides its **manufacturer-logo bitmap** (the Generic vehicle name remains text) and disables the car-description Help for that vehicle. Normal racer cars continue to have their manufacturer logos and Help.

Catch Up policy is based on whether the current race mode offers a pre-race choice. Quick Game express (type 2), Free Run (type 6) and the Championship / Hot Pursuit event trees (types 8/9) default to **Catch Up off** because no control is available; this remains true with Advanced Settings **Disable Catch Up** left unchecked. Challenge setup modes (types 3/4/5/7) preserve the user-selected authored value unless the global **Disable Catch Up** override is enabled. Both policies are applied after race parameters are committed and again at track activation. Recorded demo/replay parameters are untouched. AI difficulty, physical response curves and separately scripted police acceleration are unchanged. Runtime log fields `disable_catch_up_pref` and `catch_up` distinguish launcher preference from the effective race setting.

Showcase renders the main car with frontend material coefficients and the authored `CarSelectEnvMap` image. Its base material and environment reflection are separate passes. The [material renderer](src/ecstasy/ecstasye.cpp) adds the environment contribution using source alpha: `Cs * As + Cd`. The frontend blend does not use its fixed-alpha value as an additional brightness multiplier.

The floor reflection is another car instance with a mirrored transform. Following the original PAL behavior, the [frontend car renderer](src/care.cpp) selects LOD 1 and flags `0x11` for that instance. These flags disable its environment pass and exclude its transparent parts while retaining lighting. The normal car uses LOD 0 and keeps its authored environment pass, so the two instances have different gloss contributions.

The original 256 × 256 `CarSelectEnvMap` already contains soft light patches in both RGB and alpha. This explains a source of soft highlights, but the reported matte appearance of the main black body on Windows remains an open issue. The source map and verified material settings alone do not establish visual parity with PS2 on a Windows GPU.

Races build their environment map from six cameras. Original material flags determine whether a separate specular pass is present. The vertex shader keeps a precise, invariant position calculation with a fixed arithmetic order across base, specular and environment passes. The reported moving-shadow jitter on Windows remains an open issue.

Scheduled environment-camera updates survive canceled intermediate captures. The next submitted frame renders their accumulated faces at the latest pose, so a render cap or long frame cannot repeatedly skip the same reflection directions.

Car shadow projection and cinematic actor grounding use temporary owned terrain caches. Drawing another frame or view cannot replace the wheel-contact cache's selected ground polygon or reorder its polygons. Both shadow passes retain the original geometry and ground-query calculations. Skid-mark recency advances with logical captures, and airborne car detail uses the current camera's culling result.

Performance comparisons need the same game event, camera, car population, graphics settings and frame sequence. CPU capture, preparation and submission timings help locate a bottleneck; the independent presentation counter reports the application's frame rate. A software Vulkan validation run does not measure Windows GPU performance.

## Included components

The project uses the system Windows APIs, XAudio2 and the Vulkan loader. Included third-party files retain their own notices:

- [SDL2 license](third_party/SDL2/LICENSE.txt)
- [Vulkan headers license](third_party/vulkan/LICENSE.md)
- [Vulkan loader license](third_party/vulkan/LICENSE-Loader.txt)
