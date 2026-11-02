# 0001 — C99 + raylib 6.2 + zig cc, built with make

- **Status:** accepted
- **Date:** 2026-10-19

## Context
Solo developer shipping small 2D games on Steam, with AI agents writing much of the code. We need: fast iteration, tiny surface area the agents can't get lost in, cross-platform builds without a farm of machines, and full control of the binary.

## Options considered
1. **Godot / Unity** — editors, asset pipelines and console ports for free; but large APIs agents misuse, editor state that isn't plain text, and less control. 
2. **C99 + raylib** — ~620 functions in one header, examples as docs, compiles in seconds, everything is plain text an agent can read and diff. Must build our own tools (raygui), UI and save systems.
3. **C++ + SDL3** — more power and ecosystem, more complexity than these games need.

## Decision
C99 + raylib 6.2, with raygui (debug/tools), reasings (easing), rmem (pools/arenas) and rres (packing). `zig cc` as the portable compiler: one download, cross-compiles Windows binaries from any OS, same flags everywhere; plain `cc`/`gcc`/`clang` still work. Plain `make` as the build system. Dependencies pinned by commit in `tools/fetch_deps.sh`.

## Consequences
- Agents have a small, greppable source of truth (`vendor/raylib/src/raylib.h`).
- raylib 6.x's headless memory platform + software renderer lets agents and CI run the game and take screenshots without a GPU.
- We own UI, localization, save system, and input remapping. Keep them minimal.
- Linux builds need X11 dev headers and are best made on Linux/WSL or in the Steam Runtime container; macOS builds need a Mac.
- Consoles are out of scope for now.
