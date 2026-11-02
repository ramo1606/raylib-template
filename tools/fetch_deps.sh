#!/bin/sh
# fetch_deps.sh - Download pinned third-party libraries into vendor/.
# Versions are pinned on purpose: AI agents (and you) must build against a known API.
# To upgrade: change the ref here, run `make clean check`, and record an ADR in docs/decisions/.
set -eu

cd "$(dirname "$0")/.."
mkdir -p vendor

# name | repo | ref (tag or commit)
DEPS="
raylib   https://github.com/raysan5/raylib.git          6.2
raygui   https://github.com/raysan5/raygui.git          ca17a68d1a32abede1155c9826427482aa72d43f
rres     https://github.com/raysan5/rres.git            694a9201e7dc9242afecdc0e535bc09f051a4d76
reasings https://github.com/raylib-extras/reasings.git  76fb4a6e4d6037803a329c0177e42449e495d9ec
rmem     https://github.com/raylib-extras/rmem.git      7681279d9725afca4ba3fef0938eeb8dc09af130
"

echo "$DEPS" | while read -r name repo ref; do
    [ -z "$name" ] && continue
    dir="vendor/$name"
    if [ -d "$dir/.git" ]; then
        echo "== $name: already present ($(git -C "$dir" rev-parse --short HEAD))"
        continue
    fi
    echo "== $name @ $ref"
    git init -q "$dir"
    git -C "$dir" remote add origin "$repo"
    git -C "$dir" fetch -q --depth 1 origin "$ref"
    git -C "$dir" checkout -q FETCH_HEAD
done

echo "vendor/ ready"
