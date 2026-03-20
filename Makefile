# ── compilers ─────────────────────────────────────────────
CXX_LINUX   := g++
CXX_WIN     := x86_64-w64-mingw32-g++
CC_WIN      := x86_64-w64-mingw32-gcc

# ── source files (same for both platforms) ────────────────
SRCS := src/main.cpp \
        $(wildcard src/engine/*.cpp) \
        $(wildcard src/renderer/*.cpp) \
        $(wildcard src/world/*.cpp) \
        $(wildcard src/game/*.cpp) \
        $(wildcard src/ui/*.cpp) \
        third_party/glad/glad.c

# ── shared flags ──────────────────────────────────────────
INCLUDES := -Iinclude -Ithird_party
CXXFLAGS  := -std=c++17 -Wall -Wextra $(INCLUDES)

# ── Linux flags ───────────────────────────────────────────
LINUX_LDFLAGS  := -lglfw -lGL -ldl
LINUX_TARGET   := ZombieHorde

# ── Windows cross-compile flags ───────────────────────────
WIN_INCLUDES   := -Ithird_party/glfw-3.4.bin.WIN64/include
WIN_LDFLAGS := -Lthird_party/glfw-3.4.bin.WIN64/lib-mingw-w64 -lglfw3 -lopengl32 -lgdi32 \
               -static-libgcc -static-libstdc++
WIN_TARGET     := ZombieHorde.exe

# ── build dirs ────────────────────────────────────────────
BUILD_LINUX := build/linux
BUILD_WIN   := build/windows

# ── object lists ──────────────────────────────────────────
OBJS_LINUX := $(patsubst %.cpp, $(BUILD_LINUX)/%.o, \
              $(patsubst %.c,   $(BUILD_LINUX)/%.o, $(SRCS)))

OBJS_WIN   := $(patsubst %.cpp, $(BUILD_WIN)/%.o, \
              $(patsubst %.c,   $(BUILD_WIN)/%.o, $(SRCS)))

# ── phony targets ─────────────────────────────────────────
.PHONY: all debug release windows clean

all: debug

# Linux debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(LINUX_TARGET)

# Linux release build
release: CXXFLAGS += -O2 -DNDEBUG
release: $(LINUX_TARGET)

# Windows cross-compiled build
windows: CXXFLAGS += -O2 -DNDEBUG $(WIN_INCLUDES)
windows: $(WIN_TARGET)

# ── Linux link ────────────────────────────────────────────
$(LINUX_TARGET): $(OBJS_LINUX)
	$(CXX_LINUX) $(OBJS_LINUX) -o $@ $(LINUX_LDFLAGS)
	@echo "Built $(LINUX_TARGET) for Linux"

# ── Windows link ──────────────────────────────────────────
$(WIN_TARGET): $(OBJS_WIN)
	$(CXX_WIN) $(OBJS_WIN) -o $@ $(WIN_LDFLAGS)
	@echo "Built $(WIN_TARGET) for Windows"

# ── Linux compile ─────────────────────────────────────────
$(BUILD_LINUX)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX_LINUX) $(CXXFLAGS) -c $< -o $@

$(BUILD_LINUX)/%.o: %.c
	@mkdir -p $(dir $@)
	gcc -Ithird_party -c $< -o $@

# ── Windows compile ───────────────────────────────────────
$(BUILD_WIN)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX_WIN) $(CXXFLAGS) -c $< -o $@

$(BUILD_WIN)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC_WIN) -Ithird_party -c $< -o $@

# ── clean ─────────────────────────────────────────────────
clean:
	rm -rf build/ $(LINUX_TARGET) $(WIN_TARGET)
