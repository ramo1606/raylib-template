# Backlog

Sizes: **S** ≤ 2 h, **M** ≤ 1 day. Bigger = split it. Owner = agent role that implements it.
Status: todo · doing · review · done. Keep the table ordered by priority (top = next).

| ID | Title | Owner | Size | Acceptance criteria | Status |
|---|---|---|---|---|---|
| ST-1 | Verify studio setup on this machine | release-engineer | S | `make check` prints CHECK OK; `make run` opens a window; `make release TARGET=windows` produces a GUI .exe | todo |
| ST-2 | Gemini CLI connected | lead-programmer | S | `sh tools/ask_gemini.sh audit "quick sanity check"` returns findings | todo |
| GP-1 | Enemy telegraph before spawn | gameplay-programmer | S | a warning marker appears at the spawn edge 0.4 s before a dart spawns; test covers the delay | todo |
| GP-2 | Hit-stop + flash on death | technical-artist | S | 4 ticks of freeze and white flash on death, toggle in debug panel | todo |
| AU-1 | First SFX set (pickup, death, UI) | audio-designer | M | 3 rFXGen sounds wired to SimEvents with pitch variation; volume setting | todo |
| QA-1 | 10-seed soak in CI | qa-tester | S | CI runs 10 seeds × 3600 frames headless without crash | todo |

## Icebox
| ID | Title | Reason parked |
|---|---|---|
