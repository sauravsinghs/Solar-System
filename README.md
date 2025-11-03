# Solar System

Interactive 3D solar system written in C++17 with OpenGL 3.3. Explore planets, toggle follow‑camera views, and visualize true inclined, elliptical orbits.

## Highlights

- Keplerian orbits (J2000 elements) for Mercury → Neptune; follow‑camera (`F`) per body
- Earth–Moon system integrated with a lightweight, stable N‑body (leapfrog)
- Orbit guides (`O`) as inclined ellipses; Moon guide follows Earth
- Per‑planet HUD (bottom‑left) with real‑world facts when follow‑camera is active
- Atmosphere/ring overlays (Venus atmosphere, Saturn/Uranus rings)
- Asteroid belt with thousands of instanced rocks between Mars and Jupiter
- Time controls and pause; wireframe toggle; adjustable camera speed

## Controls

- Camera: W / A / S / D
- Up / Down: E / Q
- Speed: V (faster) / C (slower)
- Pause: P
- Time ±: Numpad + / Numpad -
- Follow camera cycle (planets → Moon → free): F
- Orbit guides toggle: O
- Exit: Esc

## Simulation model

- Planets use analytic Kepler motion from approximate J2000 elements (semi‑major axis, eccentricity, inclination, Ω, ω, M0)
- Axial tilt and self‑rotation applied per planet
- Moon integrated in Earth’s local frame via leapfrog for stability
- Visuals: inclined elliptical orbit paths, soft day/night terminator, ring/atmosphere overlays

## Settings

Configure `Solar System/Settings.h`:

- Key bindings, camera speed/sensitivity
- Window/viewport and clip planes
- Planet scales and rotation speeds
- Kepler epoch (`epochJD_J2000`), days/second (`keplerDaysPerSecond`), and AU scale (`keplerAUScale`)

## Build (CMake)

Clone and build:

```bash
# Windows (MinGW)
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --config Release -j

# Linux/macOS
cmake -S . -B build
cmake --build build -j
```

Run from the build directory so relative resource paths resolve:

```bash
cd build
"Solar System\Solar System.exe"   # Windows
./Solar\ System/Solar\ System     # macOS/Linux
```

Notes

- CMake fetches GLFW; GLAD, GLM, and stb are vendored in `Dependencies/`
- Post‑build step copies `Resources/` next to the executable

## Repository layout

```
Dependencies/                  # GLAD, GLFW (fetched), GLM, stb
Resources/
  Shaders/                     # default, earth, overlay, text, orbit
  Meshes/                      # sphere.obj
  Textures/                    # planets, rings, skybox, etc.
Solar System/                  # Source code (.h/.cpp)
CMakeLists.txt                 # Build entry
```

## Team contributions (3 members)

### Ujjwal Singh (S20230010245) — Rendering & GPU resources

- Engine wrappers and draw path:
  - `Solar System/ShaderProgram.h`, `Solar System/ShaderProgram.cpp`
  - `Solar System/Mesh.h`, `Solar System/Mesh.cpp`
  - `Solar System/Texture.h`, `Solar System/Texture.cpp`
  - `Solar System/Actor.h`, `Solar System/Actor.cpp`
  - `Solar System/objload.h`, `Solar System/objload.cpp`
- Shaders and effects:
  - `Resources/Shaders/default.vert`, `Resources/Shaders/default.frag`
  - `Resources/Shaders/earth.vert`, `Resources/Shaders/earth.frag`
  - `Resources/Shaders/noLight.vert`, `Resources/Shaders/noLight.frag`
  - `Resources/Shaders/overlay.vert`, `Resources/Shaders/overlay.frag`
  - `Resources/Shaders/text.vert`, `Resources/Shaders/text.frag`
  - `Resources/Shaders/orbit.vert`, `Resources/Shaders/orbit.frag`
- Utilities and third‑party glue:
  - `Solar System/stb_image.h`, `Solar System/stb_image.cpp`

### Arun M. (S20230010025) — Camera, input, simulation & orbital mechanics

- Application loop, input, and scene logic:
  - `Solar System/Game.h`, `Solar System/Game.cpp`
  - `Solar System/main.cpp`
  - Follow‑camera, HUD text rendering, orbit path system
- Camera system:
  - `Solar System/Camera.h`, `Solar System/Camera.cpp`
- Celestial bodies:
  - `Solar System/Planet.h`, `Solar System/Planet.cpp`
  - Kepler module: `Solar System/Orbital.h`, `Solar System/Orbital.cpp`
- Configuration:
  - `Solar System/Settings.h`

### Saurav Singh (S20230010219) — Windowing, build, assets & packaging

- Platform/window + GL context:
  - `Solar System/Window.h`, `Solar System/Window.cpp`
- Build system and vendor setup:
  - `CMakeLists.txt`
  - `Dependencies/GLAD/*`, `Dependencies/GLFW/*`, `Dependencies/GLM/*`, `Dependencies/stb/*` (integration)
- Assets & resources:
  - Skybox and planet textures under `Resources/Textures/`
  - Sphere mesh: `Resources/Meshes/sphere.obj`
  - Keyboard diagram and documentation updates

All source and shader files are attributed above; resources (textures/meshes) are curated under Saurav’s integration role.

## Cleaning up local build artifacts

All build output is contained in `build/`. To start fresh:

```bash
rm -rf build          # macOS/Linux
rmdir /S /Q build     # Windows
```

## Credits

Planet textures: `https://www.solarsystemscope.com/textures/`

OpenGL reference: `https://learnopengl.com/`
