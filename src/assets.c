// assets.c - see assets.h
#include "assets.h"

#include "rres.h"
#include "rres-raylib.h"

#include <stdbool.h>
#include <stdio.h>

static struct
{
    bool hasPack;
    char rresPath[256];
    char looseDir[256];
    rresCentralDir dir;
} assets = {0};

void AssetsInit(const char *rresPath, const char *looseDir)
{
    snprintf(assets.looseDir, sizeof(assets.looseDir), "%s", looseDir);
    snprintf(assets.rresPath, sizeof(assets.rresPath), "%s", rresPath);

    if (FileExists(rresPath))
    {
        assets.dir = rresLoadCentralDirectory(rresPath);
        assets.hasPack = (assets.dir.count > 0);
        TraceLog(LOG_INFO, "ASSETS: using pack %s (%u entries)", rresPath, assets.dir.count);
    }
    else
        TraceLog(LOG_INFO, "ASSETS: no pack, using loose files in %s/", looseDir);
}

void AssetsShutdown(void)
{
    if (assets.hasPack) rresUnloadCentralDirectory(assets.dir);
    assets.hasPack = false;
}

static Image LoadImageFromPack(const char *name)
{
    Image img = {0};
    unsigned int id = rresGetResourceId(assets.dir, name);
    if (id == 0) return img;

    rresResourceChunk chunk = rresLoadResourceChunk(assets.rresPath, id);
    if (UnpackResourceChunk(&chunk) == 0) img = LoadImageFromResource(chunk);
    rresUnloadResourceChunk(chunk);
    return img;
}

Texture2D AssetsLoadTexture(const char *name)
{
    Image img = {0};

    if (assets.hasPack) img = LoadImageFromPack(name);

    if (img.data == NULL)
    {
        const char *path = TextFormat("%s/%s", assets.looseDir, name);
        if (FileExists(path)) img = LoadImage(path);
    }

    if (img.data == NULL)
    {
        // Loud magenta checker: obvious in screenshots, never a crash
        TraceLog(LOG_WARNING, "ASSETS: '%s' not found, using placeholder", name);
        img = GenImageChecked(16, 16, 4, 4, MAGENTA, BLACK);
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);
    return tex;
}
