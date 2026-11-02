#!/bin/sh
# pack_assets.sh <dist_dir> - Put game data next to the executable.
#
# If rrespacker (raylib technologies, has a command-line mode) is on PATH, all of assets/
# is packed into a single data.rres. Otherwise the loose assets/ folder is copied, which the
# game also supports (see src/assets.c), so a release never depends on the packer.
set -eu

DIST="${1:?usage: pack_assets.sh <dist_dir>}"
cd "$(dirname "$0")/.."

if command -v rrespacker >/dev/null 2>&1; then
    echo "packing assets/ -> $DIST/data.rres"
    # Check `rrespacker --help` for the flags of your installed version and adjust here.
    rrespacker --input assets --output "$DIST/data.rres"
else
    echo "rrespacker not found: copying loose assets/ (fine for dev and early access)"
    mkdir -p "$DIST/assets"
    cp -R assets/. "$DIST/assets/"
    rm -f "$DIST/assets/README.md"
fi
