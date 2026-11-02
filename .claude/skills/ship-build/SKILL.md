---
name: ship-build
description: Produce verified release builds for Windows, Linux and macOS with zig cc and package them for distribution (Steam, itch.io, playtesters). Use when the human asks for a build, demo build or release candidate.
argument-hint: "[windows|linux|macos|all] [version]"
---

# Ship build: $ARGUMENTS

1. **Gate** — on a clean `main`: `git status` clean, `make check` → `CHECK OK`, `code-reviewer` has no open BLOCKERs.
2. **Version** — bump `GAME_VERSION` in `src/game.h` (semver: demo `0.x`, launch `1.0.0`), commit `release: vX.Y.Z`, tag `vX.Y.Z`.
3. **Build per platform**
   - Windows (from any host): `make dist TARGET=windows` → `build/dist/windows/`. Zig bundles the MinGW headers, so no Windows machine is needed to *build*; you still need one (or Proton/Steam Deck) to *test*.
   - Linux: build on Linux/WSL: `make dist TARGET=linux`. For maximum compatibility build inside the Steam Runtime "sniper" SDK container. Requires X11 dev headers (`libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`).
   - macOS: build on a Mac: `make dist TARGET=macos`; app bundle + signing/notarization are a separate step (see docs/PLAYBOOK.md) — skip macOS until the game earns it.
4. **Verify each artifact**
   - `file` shows the right format (PE32+ GUI for Windows).
   - Release builds have no `GAME_DEBUG`: `strings <exe> | grep -c "DEBUG (F1)"` must be 0.
   - Data is present: `data.rres` or `assets/` next to the exe.
   - Record size and `sha256sum` in `docs/production/releases.md`.
5. ★ **Human smoke test** on real hardware: launch, play one round with keyboard and with a gamepad, alt-tab, resize, quit. Steam Deck if targeting Deck Verified.
6. Zip each platform folder as `<game>-<version>-<platform>.zip` in `build/dist/` for itch/testers, or hand off to the `steam-release` skill.
