---
name: ask-gemini
description: Get an independent second opinion from Gemini CLI - whole-repo audits that need a huge context window, art/readability critique of screenshots, Steam market and competitor research with Google Search, and marketing copy. Use for big diffs before merge, visual reviews, research questions, or when stuck after two failed attempts.
allowed-tools: Bash(sh tools/ask_gemini.sh:*), Bash(gemini:*), Read
---

# Ask Gemini (second brain)

Claude Code is the builder; Gemini CLI is the independent reviewer and researcher. Different model families catch different mistakes, and Gemini's large context lets it read the whole repo at once.

Always call through the wrapper (it runs Gemini headless, read-only, from the repo root):

```sh
sh tools/ask_gemini.sh <mode> "<question>" [files...]
```

| Mode | Gemini agent used | Use for |
|---|---|---|
| `audit` | codebase-auditor | Whole-repo review: architecture drift, UB, leaks, determinism leaks, dead code. Pass nothing or a subfolder. |
| `art` | art-director | Critique screenshots: `sh tools/ask_gemini.sh art "Is the player readable?" build/shots/smoke.png` |
| `market` | market-analyst | Steam research: comparable games, price points, tags, launch timing, wishlist benchmarks (uses Google Search). |
| `write` | narrative-writer | Store page copy, devlog drafts, item/enemy names, flavour text. |
| `ask` | (none) | Any other question. |

How to use the answer:
- Treat Gemini's output as **input to a decision, not a decision**. Verify every concrete claim (file:line, API name) yourself before acting.
- Summarize agreements and disagreements with your own view for the human in ≤ 8 lines.
- Save research outputs worth keeping under `docs/research/<date>-<topic>.md`.

If `gemini` is not installed or not authenticated, say so and continue without it; never block work on it.
