---
name: steam-release
description: Steam publishing workflow - Steamworks app setup checklist, SteamPipe depot configuration and upload commands, store page and Next Fest readiness, and release-day checklist. Only run when the human explicitly asks to prepare a Steam upload or release.
disable-model-invocation: true
argument-hint: "[setup|upload|store-page|launch-checklist]"
---

# Steam release: $ARGUMENTS

Safety: the human owns the Steamworks account. Never ask for, type, or store passwords or Steam Guard codes. Prepare files and commands; the human runs `steamcmd`. Never set a build live on the default branch without an explicit "yes, set live".

## setup (once per game)
- [ ] Steam Direct fee paid, app created, App ID + depot IDs (one per OS) noted in `steam/README.md`.
- [ ] Copy `steam/app_build.vdf.template` → `steam/app_build_<appid>.vdf`, fill App ID / depot IDs. Same for depot templates.
- [ ] Steamworks SDK unzipped into `steam/sdk/` (gitignored). `steamcmd` installed.
- [ ] Optional Steamworks API: thin C wrapper `src/platform_steam.c` using the SDK's flat C API, compiled only with `USE_STEAM=1`; `steam_appid.txt` for local runs only, never shipped.

## upload
1. Run the `ship-build` skill first; artifacts must be in `build/dist/<platform>/`.
2. Update `steam/app_build_<appid>.vdf` `"desc"` with `vX.Y.Z <git sha>`.
3. Give the human the command:
   `steamcmd +login <their_username> +run_app_build "$(pwd)/steam/app_build_<appid>.vdf" +quit`
4. After upload: the human sets the build live on a **beta branch** first (e.g. `playtest`), tests via Steam on a clean machine/Deck, then promotes.

## store-page (start ≥ 6 months before launch when possible)
- [ ] Capsule images (header, small, main, vertical, library assets) — the human or an artist makes them; AI-generated art must be disclosed in the Steam content survey.
- [ ] 5+ screenshots from real gameplay (use `--shot` with good seeds), trailer ≤ 60 s showing gameplay in the first 5 s.
- [ ] Short description (hook in one line), long description, tags (get candidates from `ask-gemini market`).
- [ ] "Coming soon" page live → collect wishlists; plan a demo for a Steam Next Fest.

## launch-checklist
- [ ] Release build verified on Windows, Linux/Deck (Proton or native), controller-only playable.
- [ ] Achievements (if any) tested on a beta branch.
- [ ] Price and regional pricing set; launch discount decided.
- [ ] Devlog/announcement drafted (`/devlog`), press/streamer list contacted 2 weeks before.
- [ ] Day-0 patch plan: who watches crash reports and the community hub for 72 h (you).
