# 🧟 ZombieHorde

> A first-person zombie survival shooter built from scratch in C++17 and OpenGL 3.3 Core.  
> No engine. No shortcuts. Just code, math, and a lot of undead.

![Platform](https://img.shields.io/badge/platform-Linux-informational?style=flat-square)
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
https://github.com/user-attachments/assets/01eac872-bf32-4477-9b7e-e50dc434906d

---

## Building

### Requirements

- Linux (tested on Linux Mint)
- `g++` with C++17 support
- `libglfw3-dev`
- `libgl-dev`
- GLAD (included in `third_party/`)
- GLM (included in `third_party/`)
- stb_image (included in `third_party/`)

### Install dependencies

```bash
sudo apt update
sudo apt install build-essential libglfw3-dev libgl-dev
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
└── third_party/      # GLAD, GLM, stb_image
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
