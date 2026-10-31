# raficraft

A Minecraft-inspired project made from scratch in C++ and OpenGL. Explore procedurally generated grass, dirt, and stone terrain on foot, jump over obstacles, and break or place blocks with a centered crosshair and a targeted-block outline. Chunks stream in as you move through the world. Each chunk is 16 by 256 by 16 blocks, and only exposed block faces are rendered, including at chunk boundaries. Terrain uses seeded Simplex fractal noise from FastNoiseSIMD.

![Minecraft Clone screenshot](img/readme.png?v=70a539f)

## Download and play

Download a Linux or Windows x86_64 archive from [Releases](https://github.com/rafi-the-dev/raficraft/releases), then extract the entire archive.

- Linux: open `Play.desktop` from the extracted folder. Your file manager may ask you to allow launching it. You can also run `./play.sh`.
- Windows: open `mygame.exe` from the extracted folder. The game starts without a console window. `Play.bat` remains available as a command-line launcher.

Keep the executable and `resources/` together. The launchers select the correct working directory. These are stripped Release builds with compiler optimization and link-time optimization enabled. The Linux desktop launcher starts without a terminal window. Windows builds include the compiler runtime through static linking; Linux builds require compatible system libraries.

A graphics driver supporting OpenGL 4.6 and `GL_ARB_bindless_texture` is required on both platforms. The current renderer draws each chunk in one instanced call with a texture index per face. This uses non-uniform bindless sampler selection, which requires driver support such as `GL_NV_gpu_shader5`. `GL_ARB_bindless_texture` alone does not guarantee correct rendering: see the [Khronos extension specification](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_bindless_texture.txt). Startup checks only for `GL_ARB_bindless_texture`; other GPU compatibility remains unverified.

The Linux x86_64 package requires glibc 2.38 or newer, libstdc++ providing `GLIBCXX_3.4.32`, and X11 libraries. Windows packages use MinGW-w64 with static GCC/C++ runtimes and require the Windows Universal C Runtime.

## Start a world

The game opens to a menu with a visible mouse cursor. Click **Play**, click the seed field, then enter a signed 32-bit integer. The default seed is `1337`. Backspace deletes characters. Click **Create World** to generate terrain and capture the mouse. Invalid or out-of-range seeds show an error. Click **Quit** to exit from the main menu.

The crosshair, camera controls, player movement, and terrain generation activate after creating a world. The menu uses GLUI and GL2D; its font is bundled in `resources/font/`.

## Controls

- Mouse: look around.
- W/A/S/D: walk forward, left, backward, and right.
- Space: jump while on the ground.
- Left Shift: sprint.
- Left mouse button: break the targeted block within five blocks (one block per click).
- Right mouse button: place a dirt block against the targeted face within five blocks (one block per click). Placement requires an empty cell that does not overlap the player.
- A wireframe outline shows the targeted block.
- Escape: quit.

## World and gameplay

Player movement includes gravity and collision with solid blocks. Terrain renders within 32 chunks of the player in each horizontal direction, with chunk data retained within 34 chunks. Chunks outside the camera frustum are skipped during drawing. The projection far plane scales with render distance. Chunk generation and mesh updates are limited to two each per frame, so distant terrain fills in gradually. Distant chunks unload and regenerate from the selected seed when revisited.

Block edits stay in memory while their chunk remains loaded. There is no world saving, so edits are lost when a chunk unloads or the game exits. zstd is vendored and linked for future saving work, but the saving header is only a placeholder.

## Build and run

You need a C++17 compiler, CMake 3.15 or newer, and the graphics support listed above. Dependencies are vendored in `thirdparty/`. Linux needs the X11 development libraries for GLFW; Windows can use Visual Studio or MinGW-w64.

```sh
cmake -S . -B build-dev
cmake --build build-dev
./build-dev/mygame
```

Development builds load resources from the source tree. Keep `resources/font/ANDYB.TTF` available for menu text.

## Build a release

For Linux with a single-configuration generator:

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -DRAFICRAFT_PORTABLE=ON
cmake --build build-release --config Release --parallel
cmake --install build-release --config Release --strip --prefix dist/raficraft-linux-x86_64
```

For native Windows using Visual Studio:

```sh
cmake -S . -B build-release -DRAFICRAFT_PORTABLE=ON
cmake --build build-release --config Release --parallel
cmake --install build-release --config Release --strip --prefix dist/raficraft-windows-x86_64
```

To cross-compile Windows x86_64 on Linux with MinGW-w64:

```sh
cmake -S . -B build-windows-release -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64.cmake -DCMAKE_BUILD_TYPE=Release -DRAFICRAFT_PORTABLE=ON
cmake --build build-windows-release --config Release --parallel
cmake --install build-windows-release --config Release --strip --prefix dist/raficraft-windows-x86_64
```

Package the entire installed folder and use its launcher to run the game. `RAFICRAFT_PORTABLE=ON` uses the bundled `resources/` directory instead of an absolute source path.

Portable Release builds require a compiler with link-time optimization support. They use the compiler's standard Release optimization settings without fast-math or a machine-specific CPU target. FastNoiseSIMD's CPU backends are kept outside link-time optimization to preserve their separate vector types. Install with `cmake --install ... --strip` to remove debug symbols from packaged executables. Release builds disable the OpenGL debug context and synchronous debug callbacks. Windows portable builds use the GUI subsystem; Linux packages include a `Terminal=false` desktop launcher.
