# Studio Playbook — solo, AI-assisted, raylib + C99, shipping on Steam

This is the operating manual for the studio. `CLAUDE.md` is the short version agents read every session; this is the long version for you.

---

## 1. Operating model

**You are the creative director and the only person who can say "this is fun" or "this ships".** AI agents are a team that plans, writes, tests and reviews — fast, tireless, and without taste. The setup is built around one idea:

> Agents may only claim work is done when a machine has verified it (tests, headless smoke run, screenshots) — and feel/fun is only ever verified by you.

### Two AIs, two jobs

| | Claude Code | Gemini CLI |
|---|---|---|
| Role | **Builder & orchestrator** | **Independent second brain** |
| Does | edits code, runs `make`, delegates to role agents, owns git branches and the workflow | whole-repo audits, screenshot critique, Steam/market research with Google Search, copywriting |
| Writes files? | yes | no (read-only by convention) |
| Config | `CLAUDE.md`, `.claude/agents`, `.claude/skills`, `.claude/settings.json` | `GEMINI.md`, `.gemini/agents`, `.gemini/commands` |
| Called how | you talk to it | Claude calls it via `tools/ask_gemini.sh` (skill `ask-gemini`), or you run `gemini` yourself (`/audit`, `/art-review`, `/steam-research`) |

Why two model families: they make *different* mistakes. A reviewer that didn't write the code, and runs on a different model, catches more. Gemini's large context also lets it read the entire repo in one go, which is ideal for drift audits.

---

## 2. The team (Claude Code subagents)

| Agent | Model | Can edit? | Use it for |
|---|---|---|---|
| `producer` | sonnet | docs only | "what next?", scope cuts, backlog, roadmap |
| `lead-programmer` | opus | yes | architecture, systems, perf, plans for large features, raylib quirks |
| `gameplay-programmer` | sonnet | yes | day-to-day features and bug fixes in `sim.c`/`game.c` |
| `game-designer` | opus | docs only | mechanics, GDD, tuning numbers, playtest analysis |
| `technical-artist` | sonnet | yes | palette, shaders, juice, particles, asset pipeline |
| `audio-designer` | sonnet | yes | SFX/music pipeline, event→sound wiring, mix |
| `qa-tester` | haiku | tests + QA log | `make check`, multi-seed soak, screenshot inspection, repro + failing test |
| `code-reviewer` | opus | **no** | review every diff before merge |
| `release-engineer` | sonnet | yes | builds, packaging, Steam depots, checklists |

Model choice is a cost lever: opus where judgment matters (architecture, design, review), sonnet for implementation, haiku for repetitive verification. Change the `model:` line in any agent file to rebalance. Agents with `memory: project` keep notes in `.claude/agent-memory/<agent>/` — commit them; that's your studio's institutional memory.

**Rules for adding agents:** add one only when you've given the *same instructions* three times. Keep descriptions short (they're always in context); put detail in the body.

---

## 3. Skills (your slash commands and shared know-how)

| Skill | Type | What it does |
|---|---|---|
| `/feature <item>` | workflow | frame → design → plan → implement → `make check` → QA → review → ★ your playtest → merge + devlog |
| `/prototype <idea>` | workflow | time-boxed greybox in a git worktree to answer one "is it fun?" question; parallel variants A/B |
| `/playtest` | workflow | structured session → findings → tuning/backlog |
| `/devlog` | workflow | weekly update from git + screenshots → Steam/social drafts |
| `/ship-build` | workflow | versioned, verified release builds per platform |
| `/steam-release` | workflow (manual only) | Steamworks setup, uploads, store page, launch checklists |
| `/new-game <title>` | workflow (manual only) | new project from this template + pitch session + roadmap |
| `raylib-c99` | reference (auto) | how to look up the real API + patterns + pitfalls; preloaded into programmer agents |
| `juice-pass` | reference (auto) | game-feel recipes and checklist |
| `ask-gemini` | tool (auto) | calls Gemini in audit / art / market / write mode |

★ = stops and waits for you.

---

## 4. The verification loop (why this setup works with AI)

AI is good at writing plausible code and bad at knowing whether a game *looks* or *plays* right. So the repo gives agents eyes and a referee:

1. **Pure simulation** (`sim.c`): deterministic, no raylib calls → unit-testable in milliseconds (`make test`).
2. **Headless build** (`TARGET=headless`): raylib 6.x's memory platform + `rlsw` software renderer. Runs on any machine or CI runner, no GPU, no window.
3. **Autoplay bot + screenshots**: `--autoplay --frames N --shot path --shot-every N` plays the game and writes PNGs. Agents *Read the images* (Claude and Gemini are multimodal) and prints a grep-able `SMOKE frames=… score=…` line.
4. **Hooks** enforce it: formatting after every edit; the Stop hook refuses to end a turn while unit tests fail.
5. **You** close the loop on feel with `/playtest`.

When you add a mechanic, also teach `GameBotInput` to use it — otherwise the smoke test can't see it.

Known headless limitations (raylib 6.2, found while building this template): `DrawPoly` doesn't render (use `DrawTriangle`), render-texture readback returns zeros, and screen readback is BGR/bottom-up (handled in `GameSaveFrame`). New quirks go into `docs/conventions.md`.

---

## 5. Rhythm

**Each session (30 s):** the SessionStart hook prints the milestone, next backlog items and branch. Ask the `producer` "what next?" if unsure.

**Each item (1–8 h):** `/feature <ID>`. One item per branch. `/clear` between items so context stays small and cheap.

**Daily (5 min):** play the latest build yourself for 5 minutes. Write one line in the playtest log even if nothing changed.

**Weekly:**
- `/devlog` → post it (consistent public updates build wishlists).
- `sh tools/ask_gemini.sh audit "weekly drift check"` → fix BLOCKERs, update conventions where the code is right and the doc is wrong.
- `producer` reviews the roadmap; cut scope early rather than late.

**Per milestone:** `/playtest` with at least 3 other people, `/ship-build`, an ADR for anything you'd regret.

---

## 6. Parallel work without chaos

- **Parallel = isolated.** Use git worktrees (Claude Code supports `isolation: worktree` for subagents, and you can open a session per worktree) for prototype variants, an art pass while gameplay continues, or an experimental refactor.
- **Never** let two agents edit the same files at the same time. Split by file ownership: gameplay in `sim.c`, look in `game.c` draw functions / shaders, audio in its own module.
- Good parallel combos: `qa-tester` soak + `game-designer` playtest analysis; Gemini market research while Claude implements; two `/prototype` variants.
- Review merges one at a time, `make check` after each.

---

## 7. Cost and context hygiene

- Keep `CLAUDE.md` short (it loads every turn). Move details into skills (loaded on demand) and agent bodies (loaded only when the agent runs).
- Delegate noisy work (builds with long logs, soak tests, codebase searches) to subagents so only summaries come back.
- Use Gemini for "read everything" tasks instead of filling Claude's context with the whole repo.
- Plan mode before anything M-sized: a 2-minute plan saves an hour of wrong code.
- If a fix fails twice, stop and rethink (or ask Gemini) instead of looping.

---

## 8. Git discipline

- `main` always passes `make check`. Work on `feat/*`, `fix/*`, `proto/*` branches.
- Small commits with clear messages; agents commit, **you** merge (merges and pushes are on the "ask" permission list).
- Force-push and `reset --hard` are denied in settings. `vendor/` is read-only to agents.
- Tag every build you give to anyone: `vX.Y.Z`.

---

## 9. Shipping on Steam (the business side)

- **Steam page early.** Wishlists come from time + visibility. Aim to have a "Coming Soon" page months before launch, with real gameplay screenshots (use `--shot` with good seeds) and a short gameplay-first trailer.
- **Demo + Steam Next Fest** is the biggest free visibility event for small games; plan a polished demo for one edition.
- **Market reality check** before committing to a game: `/new-game` runs a Gemini market pass on comparables (price, review counts, tags, player complaints). Prefer genres where a small, polished game can stand out.
- **AI disclosure**: Steam asks about AI-generated content. Track it per asset in `docs/art/asset-manifest.md` from day one.
- **Licences**: every font, sound, sprite and music track needs a recorded licence that allows commercial use.
- Build Steamworks features (achievements, cloud saves) behind `USE_STEAM` so itch/web builds still compile.

---

## 10. What stays human

AI can draft all of these, but you decide: the hook and pillars, whether it's fun, the art direction, the music, the price, the trailer cut, and when to stop adding things. The agents are configured to stop and ask at exactly those points.

---

## 11. Growing the setup

- A prompt you've typed 3 times → make it a skill (`.claude/skills/<name>/SKILL.md`).
- Instructions you've given an agent 3 times → put them in its file or its memory.
- A rule agents keep breaking → turn it into a hook (deterministic) instead of more CLAUDE.md text (advisory).
- For your second game: move the generic agents/skills into a personal Claude Code **plugin** (or `~/.claude/`) and keep only game-specific ones in the repo.
- Recurring jobs (nightly 10-seed soak, weekly devlog draft) fit CI (`.github/workflows/check.yml` already soaks 10 seeds) or Claude scheduled tasks.
