---
name: feature
description: End-to-end workflow to take one backlog item from idea to verified, reviewed, committed code using the studio's role agents. Use when the human says "/feature <item>", "build the next item", or "implement <feature>".
argument-hint: "[backlog id or short feature description]"
---

# Feature pipeline: $ARGUMENTS

You are the orchestrator. Delegate each stage to the named subagent, keep the main conversation for decisions, and stop for the human at the ★ gates.

1. **Frame** — Read the item in `docs/production/backlog.md`. If it doesn't exist, ask the `producer` agent to add it with size and acceptance criteria. Anything bigger than M gets split first.
2. **Design** (skip for pure tech/bugfix) — `game-designer` writes or updates a short note in `docs/design/` with the intended behaviour, tuning numbers and the pillar it serves.
   ★ Show the human the 3–5 line summary and get a yes.
3. **Plan** — create a branch `feat/<slug>`. For M-sized or architectural items, `lead-programmer` writes `docs/tech/plans/<slug>.md`; otherwise plan inline in plan mode. The plan lists files, structs/functions, tests, and how it will be verified.
4. **Implement** — `gameplay-programmer` (or `lead-programmer` for systems, `technical-artist` for visuals). Small commits on the branch.
5. **Verify** — `make check` must print `CHECK OK`. Read the smoke screenshots yourself. Then `qa-tester` runs its standard pass.
6. **Review** — `code-reviewer` on the branch diff. Fix all BLOCKERs, then re-run step 5.
   For big diffs, also run the `ask-gemini` skill in *audit* mode for an independent review.
7. ★ **Playtest gate** — tell the human exactly what to try (`make run`, keys, what to look for). Feel can only be judged by a human.
8. **Close** — on approval: merge to `main` (no force push), mark the item done in the backlog, add one line to `docs/production/devlog.md`, and note any new ADR.

If any stage fails twice, stop and report what's blocking instead of looping.
