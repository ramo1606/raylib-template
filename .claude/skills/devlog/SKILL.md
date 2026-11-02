---
name: devlog
description: Write the weekly devlog / Steam event post / social update from git history, devlog.md and fresh gameplay screenshots. Use for "/devlog", weekly updates, or when asked what changed since a date.
argument-hint: "[since date, default: 7 days ago]"
---

# Devlog since: $ARGUMENTS (if empty: 7 days ago)

1. Gather: `git log --since="<since>" --oneline main`, new lines in `docs/production/devlog.md`, finished backlog items.
2. Capture 3 fresh screenshots with good seeds:
   `make TARGET=headless && ./build/headless-debug/game --autoplay --seed <n> --frames 1200 --shot build/shots/devlog_<n>.png` — Read them and pick the 2 most readable.
3. Draft in `docs/devlog/<yyyy-mm-dd>.md`:
   - Title with one concrete hook ("Enemies now telegraph their dash").
   - 3–5 short paragraphs: what's new for the player (not for the code), one behind-the-scenes detail, what's next, a call to action (wishlist / join playtest).
   - Short variants: Steam event (≤ 300 words), a 280-char social post, a Discord post.
4. Optional polish: `sh tools/ask_gemini.sh write "Tighten this devlog, keep the voice, no hype words" docs/devlog/<file>.md`.
5. ★ The human edits and posts. Never post anywhere yourself.
