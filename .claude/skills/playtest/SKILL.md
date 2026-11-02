---
name: playtest
description: Run a structured playtest session with the human (or process notes from external testers) and turn it into findings, tuning proposals and backlog items. Use for "/playtest", after a feature's playtest gate, or when the human pastes tester feedback.
argument-hint: "[build or feature being tested]"
---

# Playtest: $ARGUMENTS

1. **Prepare** — make sure `make check` passes. Write the build's git short sha and current `Tuning` values into a new entry in `docs/production/playtest-log.md`.
2. **Brief the human** (≤ 6 lines): what to play, for how long (5–10 min), what NOT to be told in advance, and to think aloud. Suggest recording the screen.
3. **Collect** — ask these, one message, numbered so they can answer fast:
   1. What were you trying to do most of the time?
   2. When did you feel most in control? Least?
   3. Anything confusing, unfair or boring? When?
   4. Best moment? Would you play another round right now (yes/no)?
   5. One thing you'd change.
   Plus the numbers: rounds played, best score, time to first death.
4. **Analyze** with the `game-designer` agent: separate observations from opinions, map each finding to a pillar, propose at most 3 changes (tuning first, features last) with expected effect.
5. **Act** — tuning changes go straight into `docs/design/tuning.md` + `SimDefaultTuning` (after the human agrees); feature changes become backlog items via the `producer`.

External tester notes (Steam Playtest, Discord, friends): paste them in, skip steps 1–3, and also tag each item with how many testers mentioned it.
