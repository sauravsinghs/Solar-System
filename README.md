# Solar System

Welcome to the Solar System Simulation! This project is a 3D simulation of the solar system using C++ and OpenGL.

## Features

- Accurate (relative to earth) representation of the day/night cycles of the planets in the solar system.
- Free fly camera for exploring the solar system.
- Ability to control the flow of time, stop it and even reverse it.
- Wireframe view mode.
- A plethora of tweakable settings to customize the experience.

### Current scope (for mid evaluation)

The project is minimal to reflect partial progress:

- Sun, Earth, and Mars only
- Basic lighting for all bodies (no advanced Earth shader)
- No moon, asteroid belt, Venus atmosphere, or planetary rings

Planned additions for the next milestone:

- Earth advanced shader (day/night, clouds, specular)
- Moon with tidally locked orbit around Earth
- Asteroid belt between Mars and Jupiter
- Venus atmosphere overlay; Saturn and Uranus rings

## Controls

- Camera movement: W / A / S / D
- Move camera up: E
- Move camera down: Q
- Speed up camera: V
- Slow down camera: C
- Pause simulation: P
- Speed up time: Numpad +
- Slow down time: Numpad -
- Exit: Esc

## The settings

Various options and preferences can be specified in the `settings.h` file.
Such as :

- Key bindings.
- Planet positions, speeds, rotations and sizes.
- Lighting color and intensity.
- Time speeds.
- Camera speed and mouse sensitivity.
- Window and viewport settings.

## The simulation

Currently, the simulation contains only the Sun, Earth and Mars.

## Shaders and lighting

For the current minimal build:

- `Resources/Shaders/default.vert`, `Resources/Shaders/default.frag`: basic per-pixel lighting (ambient + diffuse + subtle rim). Used for Earth and Mars.
- `Resources/Shaders/noLight.vert`, `Resources/Shaders/noLight.frag`: unlit textured draw. Used for the Sun and skybox.

Advanced Earth/overlay shaders have been removed for this milestone to keep the project intentionally unfinished for mid-evaluation.

## Dependencies

- C++17
- OpenGL 3.3 (or later)
- CMake 3.16+ (build system)
- A C++ toolchain:
  - Windows: MinGW-w64 (preferred) or VS Build Tools
  - Linux/macOS: system compiler (gcc/clang)

## Build and Run with CMake (cross‑platform)

Clone and build:

```bash

# Windows (MinGW)
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --parallel

# Linux/macOS
cmake -S . -B build
cmake --build build -j
```

Run from the build directory so resources resolve correctly:

```bash
cd build
"Solar System\Solar System.exe"   # Windows
./Solar\ System/Solar\ System     # macOS/Linux
```

Notes

- CMake downloads/uses GLFW automatically and links GLAD; GLM and stb are vendored.
- The CMake script copies `Resources/` next to the build so the app’s relative paths work.

## Repository layout

```
Dependencies/         # GLAD, GLFW (fetched), GLM, stb
Resources/            # Meshes, Shaders, Textures
Solar System/         # Source code (.h/.cpp), Visual Studio project (optional)
CMakeLists.txt        # CMake build entry
```

## Team contributions (3 members)

- Ujjwal Singh (S20230010245): Rendering infrastructure and resources

  - Implemented OpenGL resource wrappers and draw primitives
  - Files: `Solar System/ShaderProgram.*`, `Solar System/Mesh.*`, `Solar System/Texture.*`, `Solar System/Actor.*`
  - Shaders: `Resources/Shaders/default.*`, `Resources/Shaders/noLight.*`

- Arun M. (S20230010025): Camera, input, and scene simulation

  - Implemented free-fly camera, input handling, and frame loop
  - Assembled Sun/Earth/Mars scene and per-frame updates
  - Files: `Solar System/Camera.*`, `Solar System/Planet.*`, `Solar System/Game.*`, `Solar System/Settings.h`

- Saurav Singh (S20230010219): Windowing, build system, and assets integration
  - Set up GLFW window/context, input surface, and swap chain
  - Configured CMake, dependency fetching, and resource packaging post-build
  - Integrated skybox and curated minimal assets (sun/earth/mars/stars)
  - Files: `Solar System/Window.*`, `CMakeLists.txt`, `Resources/Meshes/sphere.obj`, `Resources/Textures/{sun.jpg, earth.jpg, mars.jpg, stars_milkyway.jpg}`

## Cleaning up local build artifacts

All build output is contained in `build/`. To start fresh:

```bash
rm -rf build          # macOS/Linux
rmdir /S /Q build     # Windows
```

## Credits

Planet textures were retrieved from https://www.solarsystemscope.com/textures/

An OpenGL resource that greatly helped : https://learnopengl.com/
