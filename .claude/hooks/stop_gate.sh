#!/bin/sh
# Stop hook: if game code or tests changed in this working tree, unit tests must pass
# before Claude may end its turn. Exit 2 = block stopping and show stderr to Claude.
# Runs only `make test` (seconds), not the full smoke run, to keep turns fast.
cd "${CLAUDE_PROJECT_DIR:-.}" || exit 0

INPUT=$(cat)
# Avoid infinite loops: if we already blocked once this turn, let it stop.
echo "$INPUT" | grep -q '"stop_hook_active": *true' && exit 0

git rev-parse --is-inside-work-tree >/dev/null 2>&1 || exit 0
CHANGED=$(git status --porcelain -- src tests 2>/dev/null)
[ -z "$CHANGED" ] && exit 0

if ! OUT=$(make --no-print-directory test 2>&1); then
    echo "Unit tests are failing after your changes. Fix them (or explain why) before finishing:" >&2
    echo "$OUT" | grep -E "FAIL|error|TESTS" | head -20 >&2
    exit 2
fi
exit 0
