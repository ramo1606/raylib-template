# assets/

Source-of-truth game data, loaded by `src/assets.c`.

| Folder | Contents | Notes |
|---|---|---|
| `sprites/` | PNG sprites and atlases | Pixel art at 1x; the game renders at 640x360 and scales up |
| `audio/` | `.wav` SFX (from rFXGen) and `.ogg`/`.qoa` music | Keep SFX short, mono, 44.1 kHz |
| `fonts/` | `.ttf` or raylib `.fnt` | Check the licence allows embedding in a commercial game |
| `shaders/` | `.fs` / `.vs` GLSL 330 | Keep a `#version 100` variant if you ship to web |

Rules (enforced by the technical-artist agent):

1. Every file is listed in `docs/art/asset-manifest.md` with **source and licence**. No unlicensed or unknown-origin assets, ever. AI-generated assets are marked as such (Steam requires an AI content disclosure).
2. Lowercase, `snake_case` names, no spaces: `sprites/player_idle.png`.
3. A missing file never crashes: the loader shows a magenta checker. Seeing magenta in a smoke screenshot means a broken path.
4. Release builds pack this folder into `data.rres` with `tools/pack_assets.sh`.

`sprites/player.png` is intentionally missing in the template, so you can see the placeholder in action.
