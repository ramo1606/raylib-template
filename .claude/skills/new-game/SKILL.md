---
name: new-game
description: Start a new game project from this studio template - rename, reset docs, run the pitch session and create the first roadmap. Use only when the human says "/new-game" or explicitly asks to start a new game.
disable-model-invocation: true
argument-hint: "[working title]"
---

# New game: $ARGUMENTS

1. Make sure you are in a **new repo created from the template**, not in `raylib-template` itself (`git remote get-url origin` must not end in `raylib-template.git`). If you are in the template, stop and tell the human to run:
   `gh repo create <slug> --private --template ramo1606/raylib-template --clone` (or GitHub's "Use this template" button), then open Claude Code in that folder.
   Then run `sh tools/fetch_deps.sh` and `make check` to prove the starter works on this machine.
2. Rename: `GAME_TITLE` in `src/game.h`, `GAME ?=` in `Makefile`, window title. Keep the starter game until the first prototype replaces it.
3. **Pitch session** with the `game-designer` agent (interactive, the human answers):
   - What's the one-sentence hook? What does the player *do* every few seconds?
   - 3 pillars (feelings/qualities, not features).
   - 3 comparable Steam games — then `sh tools/ask_gemini.sh market "<comparables>: price, review counts, release year, tags, what players praise/complain about"`.
   - Target length of a session and of the whole game; target platforms.
   - Scope budget: months available, hours/week.
   Write `docs/design/pitch.md`.
4. With the `producer` agent: roadmap with milestones (prototype, vertical slice, Steam page, demo/Next Fest, launch) sized to the scope budget, and a first backlog of ≤ 10 S/M items aimed at the first prototype.
5. First ADR: `docs/decisions/0001-tech-stack.md` (already filled for raylib 6.2 + C99 + zig cc; adjust if anything differs).
6. Suggest running `/prototype` on the core mechanic next.
