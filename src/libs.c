// libs.c - The ONE translation unit that compiles single-header library implementations.
// Compiled with relaxed warnings (it's third-party code). Never put game code here.
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define RMEM_IMPLEMENTATION
#include "rmem.h"

#define RRES_IMPLEMENTATION
#include "rres.h"

#define RRES_RAYLIB_IMPLEMENTATION
#include "rres-raylib.h"

// reasings.h is static-inline by default: include it where needed, no implementation TU.
