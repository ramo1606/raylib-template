---
name: raylib-c99
description: Reference for writing correct raylib 6.2 + C99 code in this repo - how to look up the real API, standard patterns (virtual resolution, Camera2D, sounds, shaders, input, save files) and known pitfalls. Use whenever writing or reviewing code that calls raylib, raygui, reasings, rmem or rres.
---

# raylib 6.2 + C99 field guide

## 1. Never guess the API — look it up (fast)

```sh
grep -n "RLAPI .*Sound" vendor/raylib/src/raylib.h          # all Sound functions
grep -n "RAYGUIAPI" vendor/raygui/src/raygui.h | grep -i slider
grep -rln "LoadShader" vendor/raylib/examples | head        # find a working example
grep -n "EASEDEF" vendor/reasings/src/reasings.h            # easing names
grep -n "RMEMAPI" vendor/rmem/src/rmem.h                    # pools/arenas
```
raylib examples are the official documentation. Before writing a new kind of code (shaders, audio streams, 3D, gamepad), open the closest example in `vendor/raylib/examples/<module>/` and follow it.

## 2. Patterns used in this codebase

**Virtual resolution** — draw everything into `Game.target` (640x360), then `DrawTexturePro` with a *negative source height* (render textures are y-flipped) to letterbox into the window. Mouse coordinates must be mapped back: `(GetMousePosition() - offset) / scale`.

**Camera2D** — for scrolling games:
```c
Camera2D cam = { .offset = { VIRTUAL_W/2.0f, VIRTUAL_H/2.0f }, .target = playerPos, .zoom = 1.0f };
BeginMode2D(cam); /* world */ EndMode2D(); /* HUD after */
```
Smooth follow: `cam.target.x += (player.x - cam.target.x)*(1.0f - powf(0.001f, dt));`

**Sounds** — load once in `GameInit`, play from `SimEvent` flags. Overlapping copies: `LoadSoundAlias(base)`. Variation: `SetSoundPitch(s, 0.9f + 0.2f*rnd)`. Music: `LoadMusicStream` + `UpdateMusicStream(music)` every frame. `InitAudioDevice()` after `InitWindow`, `CloseAudioDevice()` before `CloseWindow`.

**Shaders** — `LoadShader(NULL, "assets/shaders/crt.fs")` (desktop GLSL 330). Uniforms: `GetShaderLocation` once, `SetShaderValue` per frame. Apply a post effect by wrapping the final `DrawTexturePro(target...)` in `BeginShaderMode/EndShaderMode`.

**Input** — the sim only sees `InputState`. Add new actions there, map keys/gamepad in `GamePollInput`, and teach `GameBotInput` to use them so smoke tests exercise the feature.

**Save files** — plain structs with a version field, written with `SaveFileData(path, &data, sizeof(data))`; on load, check `size == sizeof` and `version`. Put them under the OS user-data dir (or Steam Cloud path later), never next to the exe.

**Easing (reasings)** — signature `Ease*(t, start, change, duration)`: `EaseBackOut(t, 0, 10, 1)` goes 0 → 10 over t ∈ [0,1].

**Memory (rmem)** — see docs/conventions.md: `ObjPool` for many same-size objects, `BiStack` as scratch arena, `MemPool` for long-lived variable-size data.

**rres** — `rresLoadCentralDirectory` → `rresGetResourceId(dir, "sprites/x.png")` → `rresLoadResourceChunk` → `UnpackResourceChunk` → `LoadImageFromResource`. Already wrapped by `AssetsLoadTexture`; add `AssetsLoadSound`/`AssetsLoadFont` the same way (`LoadWaveFromResource`, `LoadFontFromResource`).

## 3. Pitfalls (append new ones to docs/conventions.md)

- `DrawTriangle` needs **counter-clockwise** vertices or nothing draws.
- Headless (`TARGET=headless`, rlsw software renderer): no input, `DrawPoly` doesn't render, render-texture readback returns zeros.
- `TextFormat` returns a static rotating buffer: copy the string if you keep it.
- `IsKeyPressed` inside the fixed-update loop can fire 0 or 2 times per frame. Sample input once per frame into `InputState`, and clear "pressed" flags after the first fixed step consumes them (current template: acceptable for menus; fix before precise-timing mechanics).
- `SetExitKey(KEY_NULL)` is set, so ESC never quits silently.
- `GetRandomValue` is fine for cosmetic randomness (particles) but never in `sim.c` — use `SimRandU32`.
- Struct literals in C99: `(Vector2){ x, y }`, designated initializers OK; no `{}` empty initializer (that's C23) — use `{ 0 }`.
