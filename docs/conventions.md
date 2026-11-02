# Code conventions (shared by Claude and Gemini)

## Language
- **C99 only.** `-std=c99`, no compiler extensions in game code, no VLAs, no C++.
- Compiles warning-free under `-Wall -Wextra -Wshadow` with both `zig cc` and `gcc`.
- `#include` order: own header, project headers, vendor headers, then libc.

## Naming (matches raylib, so game code and engine code read the same)
- Functions: `PascalCase`, module-prefixed for public API: `SimStep`, `AssetsLoadTexture`.
- Types: `PascalCase` structs with a `typedef` of the same name.
- Variables and fields: `camelCase`. Constants and macros: `UPPER_SNAKE`.
- File-local functions are `static`. Every non-static function has a prototype in a header.

## Memory
- Prefer **fixed-size arrays with an `active` flag** for game entities (see `Sim.enemies`). Small games rarely need dynamic allocation.
- If you need dynamic memory: `rmem.h`
  - `MemPool` for long-lived, variable-size allocations (level data),
  - `ObjPool` for many same-sized objects (bullets, particles),
  - `BiStack` as a per-frame / per-level scratch arena (reset, never free one by one).
- Every `Load*` has a matching `Unload*` in the same module's shutdown path.
- No allocation inside the per-frame hot path unless it goes through a pool.

## Determinism and testability
- `sim.c` never calls raylib functions, `rand()`, `time()`, or reads files. RNG is `SimRandU32` with an explicit seed.
- Float math in the sim uses `float` and fixed `dt` (`FIXED_DT`). Never feed `GetFrameTime()` into the sim.
- New gameplay rule ⇒ new test in `tests/`. A bug fix ⇒ a test that failed before the fix.

## raylib usage
- Render at the virtual resolution (`VIRTUAL_W x VIRTUAL_H`) into `Game.target`, then scale. Never draw gameplay directly to the window.
- Text for debug: `DrawText`. Text for players: a loaded `Font` (localization-ready) — never hard-code player-facing strings in `.c` files once the game has a `strings` table.
- Easing: `reasings.h` (`Ease*(t, start, change, duration)`). Prefer easing over linear lerps for anything the player sees move.
- Debug UI: `raygui` behind `#if defined(GAME_DEBUG)`. It must never appear in release builds.
- Headless build (software renderer, raylib 6.2) limitations we hit: `DrawPoly` does not render (use `DrawTriangle`, counter-clockwise), render-texture readback returns zeros (`GameSaveFrame` handles it). Add new findings here.

## Comments
- Explain *why*, not what. Keep a short header comment on each file saying what belongs in it and what doesn't.
