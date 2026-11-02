---
name: prototype
description: Time-boxed greybox prototype of a game idea or mechanic in an isolated git worktree, to answer "is this fun?" before committing to it. Use for "/prototype <idea>", game-jam style experiments, or comparing two mechanic variants side by side.
argument-hint: "[idea in one sentence]"
---

# Prototype: $ARGUMENTS

Goal: a playable answer to ONE question in hours, not days. Code quality is secondary; the question is primary.

1. With the `game-designer` agent, write `docs/design/prototypes/<slug>.md`:
   - the single question this prototype answers ("Is dodging with momentum fun for 2 minutes?"),
   - the smallest playable version (verbs, one enemy type, one goal),
   - what the human should feel if the answer is yes, and the kill criteria.
2. Create an isolated worktree on branch `proto/<slug>` so `main` stays clean. If comparing variants, create one worktree per variant (`proto/<slug>-a`, `-b`) and implement them in parallel with separate `gameplay-programmer` subagents.
3. Greybox only: rectangles, circles, debug text, `DrawText`. Put every number in `Tuning` with a debug-panel slider — prototypes are about tuning.
4. Make `GameBotInput` able to play it roughly, run `make smoke`, check the screenshots.
5. ★ Hand to the human: `make run`, a 5-question playtest sheet (from docs/production/playtest-log.md template), and the kill criteria.
6. Record the verdict (keep / iterate / kill) and what was learned in the prototype note. Kept ideas become backlog items; the prototype branch is never merged as-is.
