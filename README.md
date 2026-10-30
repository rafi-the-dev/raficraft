# Minecraft Clone

A Minecraft-inspired project made from scratch in C++ and OpenGL. Explore procedurally generated grass, dirt, and stone terrain on foot, jump over obstacles, and break blocks with a centered crosshair. Chunks stream in as you move through the world. Each chunk is 16 by 256 by 16 blocks, and only exposed block faces are rendered, including at chunk boundaries. Terrain uses seeded Simplex fractal noise from FastNoiseSIMD.

![Minecraft Clone screenshot](img/readme.png?v=70a539f)

## Download and play

Download a Linux or Windows x86_64 archive from [Releases](https://github.com/rafi-the-dev/raficraft/releases), then extract the entire archive.

- Linux: run `./play.sh` from the extracted folder.
- Windows: run `Play.bat` from the extracted folder.

Keep the executable and `resources/` together. The launchers select the correct working directory. These are optimized Release builds. Windows builds include the compiler runtime through static linking; Linux builds require compatible system libraries.

A graphics driver supporting OpenGL 4.6 and `GL_ARB_bindless_texture` is required. No NVIDIA-specific extensions are required. Faces are grouped by texture so each draw uses a dynamically uniform bindless sampler. Non-NVIDIA GPUs still need driver support for `GL_ARB_bindless_texture`. The published Linux x86_64 build requires glibc 2.38 or newer, libstdc++ providing `GLIBCXX_3.4.32`, and X11 libraries. GPU extension support is required on both platforms.

## Controls

- Mouse: look around.
- W/A/S/D: walk forward, left, backward, and right.
- Space: jump while on the ground.
- Left Shift: sprint.
- Left mouse button: break the targeted block within five blocks (one block per click).
- Escape: quit.

## World and gameplay

Player movement includes gravity and collision with solid blocks. Terrain renders within three chunks of the player in each horizontal direction, with chunk data retained within five chunks. Distant chunks unload and regenerate from the fixed seed when revisited.

Block edits stay in memory while their chunk remains loaded. There is no world saving, so edits are lost when a chunk unloads or the game exits. Block placement is not implemented yet.

## Build and run

You need a C++17 compiler, CMake 3.15 or newer, and the graphics support listed above. Dependencies are vendored in `thirdparty/`. Linux needs the X11 development libraries for GLFW; Windows can use Visual Studio or MinGW-w64.

```sh
cmake -S . -B build-dev
cmake --build build-dev
./build-dev/mygame
```

Development builds load resources from the source tree.

## Build a release

For Linux with a single-configuration generator:

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -DRAFICRAFT_PORTABLE=ON
cmake --build build-release --config Release --parallel
cmake --install build-release --config Release --prefix dist/raficraft-linux-x86_64
```

For native Windows using Visual Studio:

```sh
cmake -S . -B build-release -DRAFICRAFT_PORTABLE=ON
cmake --build build-release --config Release --parallel
cmake --install build-release --config Release --prefix dist/raficraft-windows-x86_64
```

To cross-compile Windows x86_64 on Linux with MinGW-w64:

```sh
cmake -S . -B build-windows-release -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64.cmake -DCMAKE_BUILD_TYPE=Release -DRAFICRAFT_PORTABLE=ON
cmake --build build-windows-release --config Release --parallel
cmake --install build-windows-release --config Release --prefix dist/raficraft-windows-x86_64
```

Package the entire installed folder and use its launcher to run the game. `RAFICRAFT_PORTABLE=ON` uses the bundled `resources/` directory instead of an absolute source path.
