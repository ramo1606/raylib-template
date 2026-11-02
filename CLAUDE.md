# CLAUDE.md — Studio project memory

Solo indie studio. Small 2D games in **C99 + raylib 6.2**, shipped on **Steam** (Windows first, Linux/Steam Deck second, macOS if cheap).
The human is the **creative director**: owns vision, taste, final calls, and every merge. Agents propose and execute; they don't decide scope.

@docs/conventions.md

## Commands (always use these, never invent build lines)

| Task | Command |
|---|---|
| Build + run (window) | `make run` (`ARGS="--seed 7"` to pass flags) |
| Unit tests (pure logic) | `make test` |
| Headless smoke + screenshots | `make smoke` → `build/shots/*.png`, prints a `SMOKE ...` line |
| **Definition-of-done gate** | `make check` (tests + smoke). Must print `CHECK OK` |
| Windows release (cross) | `make release TARGET=windows` |
| Shippable folder | `make dist TARGET=windows` → `build/dist/windows/` |
| Sanitizers (native debug) | `make SAN=1 run` |
| clangd database | `make compdb` |

The toolchain is `zig cc` (falls back to `cc`). Third-party code lives in `vendor/` (pinned by `tools/fetch_deps.sh`); never edit `vendor/`.

## Architecture in one breath

- `src/sim.c|h` — **pure, deterministic gameplay**. No raylib function calls, only raylib types. Input arrives as `InputState`. Everything testable lives here.
- `src/game.c|h` — presentation: scenes, drawing, particles, juice, audio, raygui debug panel (`GAME_DEBUG` builds only, F1).
- `src/main.c` — window, fixed 60 Hz timestep, CLI flags (`--autoplay --frames N --shot path --shot-every N --seed N`).
- `src/assets.c|h` — loads from `data.rres` → loose `assets/` → magenta placeholder. Never crash on a missing asset.
- `src/libs.c` — the only file with `*_IMPLEMENTATION` defines.
- Tunable numbers live in `Tuning` (sim.h), not as magic numbers in code.

## How work flows here

1. Work items come from `docs/production/backlog.md`. One item = one branch = one small PR-sized change.
2. Non-trivial work starts in **plan mode**; the plan names the files it will touch and how it will be verified.
3. After changes: `make check`, then **look at the smoke screenshots** (Read the PNGs) before claiming visual work is done.
4. Design or tech decisions that are hard to reverse get an ADR in `docs/decisions/` (template: `0000-template.md`).
5. Update `docs/production/devlog.md` with one line per finished item (feeds the weekly Steam devlog).

## Team (subagents in `.claude/agents/`)

`producer` (scope/backlog) · `lead-programmer` (architecture, engine, perf) · `gameplay-programmer` (features in sim/game) · `game-designer` (mechanics, tuning, GDD) · `technical-artist` (shaders, palettes, juice, asset pipeline) · `audio-designer` (SFX/music pipeline) · `qa-tester` (smoke runs, repro, regression tests) · `code-reviewer` (read-only review) · `release-engineer` (builds, Steam depots, store checklist).
Gemini CLI is the **second brain** for big-context audits, image critique and market research — use the `ask-gemini` skill.

## Never

- Never add a dependency, a C++ file, or a new build system without an ADR and the human's OK.
- Never commit to `main` directly, never `git push --force`, never touch `steam/config.vdf` or credentials.
- Never mark visual or "feel" work done from code alone: run `make smoke` and look at the frames, then ask the human to playtest.
- Never invent raylib API. If unsure, grep `vendor/raylib/src/raylib.h` (619 functions, it's the source of truth) or `vendor/raylib/examples/`.
