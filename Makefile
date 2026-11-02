# =====================================================================================
#  raylib 6.2 + C99 game build
#
#  make                      debug build for this machine (window)
#  make run                  build + run
#  make test                 unit tests for pure logic (no window, no raylib)
#  make smoke                headless build (software renderer) + autoplay run + screenshots
#  make check                test + smoke  <- the "definition of done" gate for agents
#  make release TARGET=windows   cross-compile a Windows release with zig cc
#  make dist TARGET=windows      release + packed assets into build/dist/windows/
#  make compdb               compile_commands.json for clangd / code-intelligence
#  make format               clang-format all game sources
#
#  TARGET = native | headless | windows | linux | macos | web
#  BUILD  = debug | release
# =====================================================================================

GAME    ?= game
TARGET  ?= native
BUILD   ?= debug
SAN     ?= 0

VENDOR  := vendor
RAYLIB  := $(VENDOR)/raylib/src

# ---- Compiler: prefer zig cc (one portable toolchain that cross-compiles C99) ---------
ZIG := $(shell command -v zig 2>/dev/null)
ifeq ($(origin CC),default)
    ifneq ($(ZIG),)
        CC := zig cc
    else
        CC := cc
    endif
endif
HOSTCC ?= $(CC)

# ---- Host detection -------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
    HOST := windows
else
    UNAME := $(shell uname -s)
    ifeq ($(UNAME),Darwin)
        HOST := macos
    else
        HOST := linux
    endif
endif

ifeq ($(TARGET),native)
    PLATFORM_OS := $(HOST)
else
    PLATFORM_OS := $(TARGET)
endif

# ---- Per-target settings ----------------------------------------------------------------
RAYLIB_SRCS := rcore.c rshapes.c rtextures.c rtext.c rmodels.c raudio.c
EXE         :=
TARGET_FLAG :=
LDLIBS      :=
RL_DEFS     := -DPLATFORM_DESKTOP_GLFW -DGRAPHICS_API_OPENGL_33

ifeq ($(TARGET),headless)
    # raylib 6.x memory platform + rlsw software renderer: no window, no GPU.
    RL_DEFS := -DPLATFORM_MEMORY -DGRAPHICS_API_OPENGL_SOFTWARE
    PLATFORM_OS := $(HOST)
    ifeq ($(HOST),windows)
        LDLIBS := -lwinmm
        EXE := .exe
    else
        LDLIBS := -lm -lpthread -ldl
    endif
else ifeq ($(PLATFORM_OS),windows)
    RAYLIB_SRCS += rglfw.c
    EXE := .exe
    LDLIBS := -lopengl32 -lgdi32 -lwinmm -lshell32
    ifneq ($(HOST),windows)
        TARGET_FLAG := -target x86_64-windows-gnu
    endif
    ifeq ($(BUILD),release)
        LDLIBS += -Wl,--subsystem,windows
    endif
else ifeq ($(PLATFORM_OS),linux)
    RAYLIB_SRCS += rglfw.c
    RL_DEFS += -D_GLFW_X11 -D_GNU_SOURCE
    LDLIBS := -lm -lpthread -ldl -lrt -lX11
    ifneq ($(HOST),linux)
        # glibc 2.28 floor keeps the binary compatible with the Steam Linux Runtime
        TARGET_FLAG := -target x86_64-linux-gnu.2.28
    endif
else ifeq ($(PLATFORM_OS),macos)
    RAYLIB_SRCS += rglfw.c
    RL_DEFS += -x objective-c
    LDLIBS := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo -framework QuartzCore
else ifeq ($(PLATFORM_OS),web)
    CC := emcc
    TARGET_FLAG :=
    RL_DEFS := -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
    EXE := .html
    LDLIBS := -sUSE_GLFW=3 -sASYNCIFY -sALLOW_MEMORY_GROWTH=1 --shell-file $(RAYLIB)/minshell.html --preload-file assets
endif

# ---- Flags --------------------------------------------------------------------------------
INCLUDES := -Isrc -I$(RAYLIB) -I$(RAYLIB)/external/glfw/include \
            -I$(VENDOR)/raygui/src -I$(VENDOR)/rmem/src -I$(VENDOR)/reasings/src -I$(VENDOR)/rres/src

WARN := -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-missing-field-initializers \
        -Werror=implicit-function-declaration -Werror=return-type

ifeq ($(BUILD),release)
    OPT := -O2 -DNDEBUG
else
    OPT := -O0 -g -DGAME_DEBUG
    ifeq ($(SAN),1)
        OPT += -fsanitize=address,undefined -fno-omit-frame-pointer
        LDLIBS += -fsanitize=address,undefined
    endif
endif

GAME_CFLAGS   := -std=c99 $(TARGET_FLAG) $(OPT) $(WARN) $(INCLUDES) $(filter-out -x objective-c,$(RL_DEFS))
LIBS_CFLAGS   := -std=c99 $(TARGET_FLAG) $(OPT) -w $(INCLUDES) $(filter-out -x objective-c,$(RL_DEFS))
RAYLIB_CFLAGS := -std=c99 $(TARGET_FLAG) -O2 -w $(RL_DEFS) -I. -I$(RAYLIB) -I$(RAYLIB)/external/glfw/include
ifneq ($(BUILD),release)
    RAYLIB_CFLAGS += -g
endif

# ---- Files --------------------------------------------------------------------------------
OUT       := build/$(TARGET)-$(BUILD)
BIN       := $(OUT)/$(GAME)$(EXE)
GAME_SRCS := $(filter-out src/libs.c,$(wildcard src/*.c src/*/*.c))
GAME_OBJS := $(patsubst src/%.c,$(OUT)/obj/%.o,$(GAME_SRCS))
LIBS_OBJ  := $(OUT)/obj/libs.o
RL_OBJS   := $(patsubst %.c,$(OUT)/raylib/%.o,$(RAYLIB_SRCS))

.PHONY: all run test smoke check release dist deps compdb format clean help print-%

all: $(BIN)

$(BIN): $(GAME_OBJS) $(LIBS_OBJ) $(RL_OBJS)
	$(CC) $(TARGET_FLAG) -o $@ $^ $(LDLIBS)
	@echo "built $@"

$(OUT)/obj/libs.o: src/libs.c | deps
	@mkdir -p $(dir $@)
	$(CC) $(LIBS_CFLAGS) -c $< -o $@

$(OUT)/obj/%.o: src/%.c | deps
	@mkdir -p $(dir $@)
	$(CC) $(GAME_CFLAGS) -MMD -MP -c $< -o $@

$(OUT)/raylib/%.o: $(RAYLIB)/%.c | deps
	@mkdir -p $(dir $@)
	$(CC) $(RAYLIB_CFLAGS) -c $< -o $@

-include $(GAME_OBJS:.o=.d)

deps:
	@test -f $(RAYLIB)/raylib.h || (echo "vendor/ missing: running tools/fetch_deps.sh" && sh tools/fetch_deps.sh)

run: $(BIN)
	cd $(OUT) && ./$(GAME)$(EXE) $(ARGS)

# ---- Quality gates --------------------------------------------------------------------------
TEST_BIN := build/tests/test_sim

test: | deps
	@mkdir -p build/tests
	$(HOSTCC) -std=c99 -O0 -g $(WARN) -Isrc -I$(RAYLIB) tests/test_sim.c src/sim.c -o $(TEST_BIN) -lm
	./$(TEST_BIN)

SMOKE_FRAMES ?= 900
smoke:
	$(MAKE) --no-print-directory TARGET=headless BUILD=debug
	@mkdir -p build/shots
	./build/headless-debug/$(GAME) --autoplay --seed 1 --frames $(SMOKE_FRAMES) --shot build/shots/smoke.png --shot-every 300

check: test smoke
	@echo "CHECK OK"

release:
	$(MAKE) --no-print-directory BUILD=release TARGET=$(TARGET)

dist: release
	@rm -rf build/dist/$(TARGET) && mkdir -p build/dist/$(TARGET)
	cp build/$(TARGET)-release/$(GAME)$(EXE) build/dist/$(TARGET)/
	sh tools/pack_assets.sh build/dist/$(TARGET)
	@echo "dist ready: build/dist/$(TARGET)"

# ---- Tooling --------------------------------------------------------------------------------
compdb: | deps
	@echo "[" > compile_commands.json
	@first=1; for f in $(GAME_SRCS) tests/test_sim.c; do \
	  [ $$first -eq 1 ] || echo "," >> compile_commands.json; first=0; \
	  printf '  {"directory": "%s", "file": "%s", "command": "clang %s -c %s"}' \
	    "$(CURDIR)" "$$f" "$(filter-out -target x86_64-windows-gnu -target x86_64-linux-gnu.2.28,$(GAME_CFLAGS))" "$$f" >> compile_commands.json; \
	done; echo "" >> compile_commands.json; echo "]" >> compile_commands.json
	@echo "wrote compile_commands.json"

format:
	clang-format -i src/*.c src/*.h tests/*.c tests/*.h

clean:
	rm -rf build

print-%:
	@echo '$*=$($*)'

help:
	@sed -n '2,17p' Makefile
