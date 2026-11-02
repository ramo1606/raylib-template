# raylib Studio Kit

A solo indie game studio template: **C99 + raylib 6.2** games for **Steam**, built with **zig cc**, developed with **Claude Code** (builder + role agents) and **Gemini CLI** (second brain).

It includes a small playable starter game ("Orbit Dodge") that proves the whole pipeline: deterministic simulation with unit tests, a headless build that plays itself and takes screenshots, a raygui debug panel, rres asset loading, and Windows cross-compilation.

Read **[docs/PLAYBOOK.md](docs/PLAYBOOK.md)** for the strategy and workflows.

## Start a new game from this template

This repository is a **GitHub template**. Never develop a game in it directly: each game gets its own repo.

- **On GitHub:** click **Use this template → Create a new repository**.
- **From a terminal:**
  ```sh
  gh repo create my-new-game --private --template ramo1606/raylib-template --clone
  cd my-new-game
  ```
- Then open Claude Code in the new folder and run `/new-game "<working title>"`.

Improvements to the studio setup itself (agents, skills, Makefile, hooks) go back into this template repo so every future game gets them.

## Quick start

```sh
# 1. Tools
#    - zig (portable C compiler): https://ziglang.org/download/  (or: pip install ziglang)
#    - make, git, clang-format, python3
#    - Linux only: sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev
#    - Claude Code and Gemini CLI (npm i -g @google/gemini-cli), both logged in

# 2. Dependencies (pinned versions)
sh tools/fetch_deps.sh

# 3. Prove it works
make check        # unit tests + headless autoplay smoke run -> CHECK OK
make run          # play: WASD/arrows, Enter, Esc = pause, F1 = debug panel
make release TARGET=windows   # Windows .exe from any OS
```

Then open Claude Code in this folder and say: *"Ask the producer what's next"*, or run `/feature ST-1`.

## Map

```
CLAUDE.md, GEMINI.md        project memory for each AI
.claude/agents/             9 role agents (producer, lead/gameplay programmer, designer, tech artist, audio, QA, reviewer, release)
.claude/skills/             /feature /prototype /playtest /devlog /ship-build /steam-release /new-game + reference skills
.claude/settings.json       permissions + hooks (auto-format, test gate, session briefing)
.gemini/agents, commands    codebase-auditor, art-director, market-analyst, narrative-writer; /audit /art-review /steam-research
src/                        sim (pure logic) · game (presentation) · main (loop/CLI) · assets (rres) · libs (single-header impls)
tests/                      unit tests for the sim, no window needed
tools/                      fetch_deps, ask_gemini, pack_assets
docs/                       playbook, conventions, design, tech, decisions (ADRs), production (roadmap, backlog, logs)
steam/                      SteamPipe templates
```

## Build targets

| Command | Result |
|---|---|
| `make` / `make run` | native debug build with window |
| `make test` | unit tests |
| `make smoke` | headless (no GPU) autoplay run → `build/shots/*.png` |
| `make check` | test + smoke (the gate) |
| `make release TARGET=windows\|linux\|macos` | optimized build |
| `make dist TARGET=…` | release + data in `build/dist/<target>/` |
| `make SAN=1 run` | AddressSanitizer + UBSan (native) |
| `make compdb` | `compile_commands.json` for clangd |

Verified while building this kit (Linux host, zig 0.17): unit tests, headless smoke with screenshots, native X11 build, Windows cross-compiled PE32+ GUI executable.
