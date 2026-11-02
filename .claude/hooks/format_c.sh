#!/bin/sh
# PostToolUse(Edit|Write): clang-format any C file Claude just touched.
# Silent on success; never blocks (formatting is a convenience, not a gate).
command -v clang-format >/dev/null 2>&1 || exit 0
command -v python3 >/dev/null 2>&1 || exit 0

FILE=$(python3 -c 'import json,sys; d=json.load(sys.stdin); print(d.get("tool_input",{}).get("file_path",""))' 2>/dev/null)

case "$FILE" in
    */vendor/*) exit 0 ;;
    *.c|*.h) [ -f "$FILE" ] && clang-format -i "$FILE" ;;
esac
exit 0
