// assets.h - Asset loading with three fallbacks, in order:
//   1. packed resources in data.rres (release builds, made by tools/pack_assets)
//   2. loose files under assets/ (development, hot-swappable)
//   3. a generated placeholder (so a missing asset never crashes a build or a smoke test)
#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"

void AssetsInit(const char *rresPath, const char *looseDir);
void AssetsShutdown(void);
Texture2D AssetsLoadTexture(const char *name); // name relative to assets/, e.g. "sprites/player.png"

#endif // ASSETS_H
