#!/bin/sh
# SessionStart: give every session the same short situational briefing (stdout -> context).
cd "${CLAUDE_PROJECT_DIR:-.}" || exit 0

echo "== Studio briefing =="
if [ -f docs/production/roadmap.md ]; then
    grep -m1 -i "current milestone" docs/production/roadmap.md 2>/dev/null
fi
if [ -f docs/production/backlog.md ]; then
    echo "Next backlog items:"
    grep -E "^\| *[A-Z]+-[0-9]+" docs/production/backlog.md | grep -v -i "| done" | head -5 |
        awk -F'|' '{ gsub(/^ +| +$/, "", $2); gsub(/^ +| +$/, "", $3); gsub(/^ +| +$/, "", $4); gsub(/^ +| +$/, "", $5); printf "  %s %s (%s, %s)\n", $2, $3, $4, $5 }'
fi
if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    echo "Branch: $(git branch --show-current)  Uncommitted files: $(git status --porcelain | wc -l | tr -d ' ')"
fi
[ -f vendor/raylib/src/raylib.h ] || echo "NOTE: vendor/ is empty - run: sh tools/fetch_deps.sh"
command -v gemini >/dev/null 2>&1 || echo "NOTE: gemini CLI not found - ask-gemini skill unavailable"
exit 0
