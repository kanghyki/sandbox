# Sandbox

## Dependencies
- CMake 3.20+
- C++17 compiler (desktop builds)
- Python 3 (for `tools/gen_scene_registry.py`)
- Network access during first configure to fetch GLFW/ImGui via CMake FetchContent
- Windows (DX11) builds require the Windows SDK and a compiler that can link `d3d11`, `dxgi`, and `d3dcompiler`

## Makefile targets
- `make build` : Configure and build desktop release
- `make run` : Run desktop build
- `make build-debug` : Configure and build desktop debug
- `make run-debug` : Run desktop debug build
- `make clean` : Remove build directories

## Windows (DX11) build
On Windows the desktop target uses DirectX 11 automatically.

Example using Visual Studio generator:
```sh
cmake -S . -B build-win -G "Visual Studio 17 2022"
cmake --build build-win --config Release
```

## Features (current)
- Scene registry with selectable scenes (8 registered by default).
- Focus viewport mode:
  - If a scene is selected at startup, UI enters focus mode automatically (full viewport).
  - In focus mode, `Reduce` button appears at top-right to restore full UI.
  - In normal UI, `Scene for Camera A` window includes a `Focus View` button to enter focus mode.

## Notes
- Desktop output is generated in the selected build directory.
