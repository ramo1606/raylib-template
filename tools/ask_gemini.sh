#!/bin/sh
# ask_gemini.sh <mode> "<question>" [files...]
#   mode: audit | art | market | write | ask
# Runs Gemini CLI headless from the repo root and prints its answer to stdout.
# Called by Claude Code (ask-gemini skill) or by you directly.
#
# Gemini runs WITHOUT auto-approval, so it can only use tools that need no confirmation
# (reading, searching). Set GEMINI_MODEL to pin a model, e.g. GEMINI_MODEL=<model-id>.
set -eu
cd "$(dirname "$0")/.."

if ! command -v gemini >/dev/null 2>&1; then
    echo "ask_gemini: 'gemini' CLI not installed (npm i -g @google/gemini-cli), skipping." >&2
    exit 3
fi

MODE="${1:?usage: ask_gemini.sh <audit|art|market|write|ask> \"question\" [files...]}"
QUESTION="${2:-}"
shift 2 2>/dev/null || shift $#

case "$MODE" in
    audit)  AGENT="@codebase-auditor " ;;
    art)    AGENT="@art-director " ;;
    market) AGENT="@market-analyst " ;;
    write)  AGENT="@narrative-writer " ;;
    ask)    AGENT="" ;;
    *) echo "unknown mode: $MODE" >&2; exit 2 ;;
esac

# Attach files with Gemini's @path syntax (works for code, markdown and images)
ATTACH=""
for f in "$@"; do ATTACH="$ATTACH @$f"; done

PROMPT="${AGENT}${QUESTION}${ATTACH}"

if [ -n "${GEMINI_MODEL:-}" ]; then
    exec gemini -m "$GEMINI_MODEL" -p "$PROMPT"
else
    exec gemini -p "$PROMPT"
fi
