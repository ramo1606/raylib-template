# Architecture

Small on purpose. One person (plus agents) must be able to hold all of it in their head.

```
            InputState (keyboard / gamepad / bot / replay)
                 │
main.c ──► fixed 60 Hz loop ──► GameFixedUpdate (game.c) ──► SimStep (sim.c)   pure, deterministic
                 │                     │  reacts to SimEvent flags
                 │                     ▼
                 └──────────────► GameDraw (game.c) ──► RenderTexture 640x360 ──► window (letterboxed)
                                         │
                                  assets.c (rres → loose files → placeholder)
```

| Module | Owns | Must not |
|---|---|---|
| `main.c` | window, loop, CLI flags, smoke/screenshot mode | contain game rules |
| `sim.c/h` | rules, entities, RNG, `Tuning`, `SimEvent` | call raylib functions, read time/files, use `rand` |
| `game.c/h` | scenes, drawing, particles, juice, audio, debug panel | change sim state except through `SimStep`/`SimInit` |
| `assets.c/h` | loading textures (and later sounds/fonts) from pack or disk | crash on missing files |
| `libs.c` | single-header library implementations | contain game code |
| `tests/` | unit tests for `sim.c` | depend on raylib functions or a window |

## Build targets
- `native` (window, OpenGL 3.3, GLFW) — daily dev.
- `headless` (raylib `PLATFORM_MEMORY` + `rlsw` software renderer) — CI and agents: runs anywhere, no GPU, produces screenshots.
- `windows` / `linux` / `macos` — release; Windows cross-compiles from any host with `zig cc`.
- `web` (emscripten) — optional, for itch.io demos.

## Decisions log
See `docs/decisions/`. Add one line here when a new module is added.
