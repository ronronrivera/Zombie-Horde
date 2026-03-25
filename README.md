# 🧟 ZombieHorde

> A first-person zombie survival shooter built from scratch in C++17 and OpenGL 3.3 Core.  
> No engine. No shortcuts. Just code, math, and a lot of undead.

![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-informational?style=flat-square)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue?style=flat-square)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3_Core-green?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-brightgreen?style=flat-square)

---

## What is this?

ZombieHorde is an open source first-person shooter where you survive endless waves of zombies in a 3D warehouse environment. Built entirely without a game engine — every system from the renderer to the AI is written by hand.

Enemies are rendered as **billboard sprites** (DOOM-style) — 2D textures that always face the camera. The world is fully 3D. This keeps the game fast and atmospheric on modest hardware while requiring zero 3D modelling.

---

## Features

- First-person movement and mouse look
- Endless wave system — each wave spawns a larger horde
- Billboard zombie sprites with walk and death animations
- Zombie seek-steering AI — they always come for you
- Shoot, reload, and manage ammo
- Point lights and distance fog for atmosphere
- HUD with health bar, ammo counter, and wave number
- Enemy radar minimap
- Fully 3D tile-based warehouse map
- All assets CC0 (free and open source)

---

## Screenshots

### Current Output

https://github.com/user-attachments/assets/55eff043-e724-42a8-9994-eb49ae852904

---

## Building on Linux

### Requirements

- Linux Mint / Ubuntu or any Debian-based distro
- `g++` with C++17 support
- `libglfw3-dev`
- `libgl-dev`
- `libassimp-dev`
- GLAD, GLM, stb_image (included in `third_party/`)

### Install dependencies

```bash
sudo apt update
sudo apt install build-essential libglfw3-dev libgl-dev libassimp-dev
```

### Compile and run

```bash
# debug build (with symbols)
make debug
./ZombieHorde

# release build (optimised)
make release
./ZombieHorde

# clean build artifacts
make clean
```

---

## Building on Windows

### Requirements

- Windows 10 or later (64-bit)
- [Visual Studio Community 2022](https://visualstudio.microsoft.com/) with C++ workload
- [Git for Windows](https://git-scm.com/download/win)
- [CMake](https://cmake.org/download/) (3.20 or later)
- [Python 3](https://www.python.org/downloads/) (optional, for some build scripts)

### Step 1 — Install Prerequisites

#### 1.1 Install Visual Studio 2022
- Download from https://visualstudio.microsoft.com/
- Run the installer and select the **"Desktop development with C++"** workload
- This includes MSVC compiler, CMake, and build tools
- Installation takes 10-20 minutes

#### 1.2 Install Git for Windows
- Download from https://git-scm.com/download/win
- Use default installation settings
- Verify installation by opening PowerShell and running:
  ```powershell
  git --version
  ```

#### 1.3 Install CMake
- Download from https://cmake.org/download/
- Run the `.msi` installer
- **Important**: Check the box to add CMake to system PATH
- Verify installation in PowerShell:
  ```powershell
  cmake --version
  ```

### Step 2 — Download GLFW Windows Binaries

1. Open PowerShell in your project root
2. Navigate to the `third_party` folder:
   ```powershell
   cd third_party
   ```
3. Download the GLFW binaries:
   ```powershell
   curl -L -o glfw-3.4.bin.WIN64.zip https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip
   Expand-Archive -Path glfw-3.4.bin.WIN64.zip -DestinationPath .
   Remove-Item glfw-3.4.bin.WIN64.zip
   cd ..
   ```

### Step 3 — Build Assimp for Windows

1. Open PowerShell
2. Clone the Assimp repository:
   ```powershell
   cd $env:USERPROFILE\Downloads
   git clone https://github.com/assimp/assimp.git
   cd assimp
   git checkout v5.3.1
   ```

3. Create a build directory and configure:
   ```powershell
   mkdir build-msvc
   cd build-msvc
   ```

4. Run CMake to configure the build:
   ```powershell
   cmake .. `
     -G "Visual Studio 17 2022" `
     -A x64 `
     -DCMAKE_BUILD_TYPE=Release `
     -DCMAKE_INSTALL_PREFIX="$env:USERPROFILE/assimp-msvc" `
     -DASSIMP_BUILD_TESTS=OFF `
     -DASSIMP_BUILD_ASSIMP_TOOLS=OFF `
     -DASSIMP_BUILD_SAMPLES=OFF `
     -DBUILD_SHARED_LIBS=OFF `
     -DASSIMP_BUILD_ZLIB=ON
   ```

5. Build Assimp:
   ```powershell
   cmake --build . --config Release -j 8
   cmake --install .
   ```
   This takes 5–15 minutes depending on your system.

### Step 4 — Copy Assimp to Your Project

1. Open PowerShell in your project root
2. Create the Assimp directories in `third_party/`:
   ```powershell
   New-Item -ItemType Directory -Force -Path "third_party/assimp-win/include"
   New-Item -ItemType Directory -Force -Path "third_party/assimp-win/lib"
   ```

3. Copy headers and library:
   ```powershell
   Copy-Item -Recurse "$env:USERPROFILE/assimp-msvc/include/assimp" -Destination "third_party/assimp-win/include/"
   Copy-Item "$env:USERPROFILE/assimp-msvc/lib/*.lib" -Destination "third_party/assimp-win/lib/"
   ```

### Step 5 — Verify Your Directory Structure

Your `third_party/` folder should now look like this:

```
third_party/
├── glad/
│   ├── glad.c
│   ├── glad.h
│   └── KHR/
│       └── khrplatform.h
├── glfw-3.4.bin.WIN64/
│   ├── include/GLFW/
│   └── lib-static-ucrt/ (or lib-vc2022/)
│       └── glfw3.lib
├── glm/
│   └── glm/
│       └── glm.hpp
├── stb/
│   └── stb_image.h
└── assimp-win/
    ├── include/assimp/
    │   ├── scene.h
    │   ├── Importer.hpp
    │   ├── config.h
    │   └── (other headers)
    └── lib/
        └── assimp.lib
```

### Step 6 — Build ZombieHorde

1. Open PowerShell in your project root
2. Create a build directory:
   ```powershell
   mkdir build-msvc
   cd build-msvc
   ```

3. Configure using CMake (or use the Makefile if supported):
   ```powershell
   cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
   ```

4. Build the project:
   ```powershell
   cmake --build . --config Release -j 8
   ```

5. The executable will be at `build-msvc/Release/ZombieHorde.exe`

**Alternatively**, if a Visual Studio project file (`.sln`) exists, you can:
- Open the `.sln` file in Visual Studio
- Select **Build → Build Solution** or press `Ctrl+Shift+B`
- Run the executable from `Release/ZombieHorde.exe`

### Troubleshooting

**CMake not found**: Ensure you added CMake to PATH. Restart PowerShell after installing.

**MSVC compiler not found**: Ensure Visual Studio is installed with the C++ workload. Check your PATH includes the VC tools.

**Missing DLLs at runtime**: Copy required DLLs to the same directory as `ZombieHorde.exe`:
- `assimp.dll` (if you built as shared library)
- `opengl32.dll` (usually in System32)

---

## Building for Windows (Cross-compile from Linux)

You don't need a Windows machine. You can produce a native `ZombieHorde.exe` directly from Linux Mint using the MinGW-w64 cross-compiler.

### Step 1 — Install the cross-compiler

```bash
sudo apt update
sudo apt install mingw-w64 cmake
```

Verify it installed:

```bash
x86_64-w64-mingw32-g++ --version
```

### Step 2 — Get GLFW Windows binaries

Download the official prebuilt GLFW binaries for Windows:

```bash
cd third_party
wget https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip
unzip glfw-3.4.bin.WIN64.zip
rm glfw-3.4.bin.WIN64.zip
cd ..
```

### Step 3 — Build Assimp for MinGW

The prebuilt Assimp binaries from GitHub are MSVC-only and won't link with MinGW. Build from source instead:

```bash
cd ~/Downloads
git clone https://github.com/assimp/assimp.git
cd assimp
git checkout v5.3.1

# create a MinGW toolchain file
cat > mingw-toolchain.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER  x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

mkdir build-mingw && cd build-mingw

cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=../mingw-toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$HOME/assimp-mingw \
  -DASSIMP_BUILD_TESTS=OFF \
  -DASSIMP_BUILD_ASSIMP_TOOLS=OFF \
  -DASSIMP_BUILD_SAMPLES=OFF \
  -DBUILD_SHARED_LIBS=OFF \
  -DASSIMP_BUILD_ZLIB=ON

make -j$(nproc)
make install
```

This takes about 5–10 minutes.

### Step 4 — Copy Assimp into your project

```bash
cd ~/Projects/3DProjects/ZombieHorde   # adjust to your project path

mkdir -p third_party/assimp-win/include
mkdir -p third_party/assimp-win/lib

cp -r ~/assimp-mingw/include/assimp  third_party/assimp-win/include/
cp    ~/assimp-mingw/lib/libassimp.a third_party/assimp-win/lib/

# copy the generated config.h (created by cmake, not in the source tree)
cp ~/Downloads/assimp/build-mingw/include/assimp/config.h \
   third_party/assimp-win/include/assimp/config.h
```

### Step 5 — Verify your third_party layout

After completing the above steps, your `third_party/` folder should look like this:

```
third_party/
├── glad/
│   ├── glad.c
│   ├── glad.h
│   └── KHR/
│       └── khrplatform.h
├── glfw-3.4.bin.WIN64/
│   ├── include/GLFW/
│   └── lib-mingw-w64/
│       └── libglfw3.a
├── glm/
│   └── glm/
│       └── glm.hpp
├── stb/
│   └── stb_image.h
└── assimp-win/
    ├── include/assimp/
    │   ├── scene.h
    │   ├── config.h
    │   └── ...
    └── lib/
        └── libassimp.a
```

### Step 6 — Cross-compile

```bash
make windows
```

This produces `ZombieHorde.exe` in your project root.

### Step 7 — Test with Wine (optional)

You can run the `.exe` on Linux without a Windows machine:

```bash
sudo apt install wine
wine ZombieHorde.exe
```

### Step 8 — Distribute to Windows users

The `.exe` is statically linked — no DLLs needed. Just zip up this folder and share it:

```
ZombieHorde_release/
├── ZombieHorde.exe
└── assets/
    ├── shaders/
    ├── textures/
    └── sounds/
```

---

## Controls

| Key / Input | Action |
|---|---|
| `W A S D` | Move |
| Mouse | Look around |
| `Left Click` | Shoot |
| `R` | Reload |
| `Shift` | Sprint |
| `Escape` | Pause / quit |

---

## Project Structure

```
ZombieHorde/
├── src/
│   ├── main.cpp
│   ├── engine/       # Window, GameLoop, Input, Camera, Audio
│   ├── renderer/     # Shader, Mesh, Texture, Billboard, Renderer
│   ├── world/        # TileMap, Collision, Lighting
│   ├── game/         # Player, Zombie, Weapon, Spawner, GameState
│   └── ui/           # HUD, Minimap, Menu, BitmapFont
├── include/          # Headers mirroring src/ structure
├── assets/
│   ├── shaders/      # GLSL vertex + fragment shaders
│   ├── textures/     # Walls, floors, zombie sprites, UI
│   └── sounds/       # Gunshots, growls, ambience
└── third_party/      # GLAD, GLM, stb_image, Assimp, GLFW-win
```

---

## Architecture

ZombieHorde is built in layers. Each layer only depends on the layer below it — never the other way around.

```
Game layer        →  Player, Zombie, Weapon, Spawner
World layer       →  TileMap, Collision, Lighting
Renderer layer    →  Shader, Mesh, Texture, Billboard
Engine layer      →  Window, GameLoop, Input, Camera
OS / OpenGL / GLFW
```

This means you can change the zombie AI without touching the renderer, or swap the lighting system without touching game logic. Every module has one job.

Zombie AI uses **seek steering** — no pathfinding grid, no A*. Each frame, a zombie computes the direction toward the player and walks that way. Simple, fast, and surprisingly scary when 30 of them do it at once.

---

## Assets & Credits

All assets are CC0 (public domain). No attribution required, but we list them anyway because the creators deserve recognition.

See [CREDITS.md](CREDITS.md) for the full list.

| Asset | Source |
|---|---|
| Zombie sprites | OpenGameArt.org |
| Wall / floor textures | Kenney Industrial Pack |
| Gunshot / impact sounds | Kenney Impact Sounds |
| Zombie growl sounds | OpenGameArt.org |

---

## Roadmap

- [x] Project structure and Makefile
- [x] Window + OpenGL context (GLFW + GLAD)
- [x] Fixed timestep game loop
- [x] FPS camera and WASD movement
- [x] 3D tile map renderer
- [ ] Billboard zombie sprites
- [ ] Zombie seek-steering AI
- [ ] Wave spawner
- [ ] Shooting and hit detection
- [ ] HUD and minimap
- [ ] Sound with OpenAL
- [ ] Main menu and game over screen

---

## Contributing

Pull requests are welcome. If you want to add a feature, open an issue first so we can discuss the design.

Please keep contributions within the existing architecture — new game systems go in `src/game/`, new rendering techniques go in `src/renderer/`, and so on.

---

## License

MIT License. See [LICENSE](LICENSE) for details.

---

_Built to learn. Built to share. Built from scratch._
