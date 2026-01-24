BUILD_DIR ?= build
DEBUG_BUILD_DIR ?= build-debug
WIN_BUILD_DIR ?= build-win
WIN_GENERATOR ?= Visual Studio 17 2022
WIN_CONFIG ?= Release

all: build

generate_scenes:
	python3 tools/gen_scene_registry.py

configure: generate_scenes
	cmake -S . -B $(BUILD_DIR) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/src/sandbox

configure-debug: generate_scenes
	cmake -S . -B $(DEBUG_BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build-debug: configure-debug
	cmake --build $(DEBUG_BUILD_DIR) --config Debug

run-debug: build-debug
	./$(DEBUG_BUILD_DIR)/src/sandbox

configure-win: generate_scenes
	cmake -S . -B $(WIN_BUILD_DIR) -G "$(WIN_GENERATOR)"

build-win: configure-win
	cmake --build $(WIN_BUILD_DIR) --config $(WIN_CONFIG)

run-win: build-win
	./$(WIN_BUILD_DIR)/src/$(WIN_CONFIG)/sandbox.exe

clean:
	rm -rf $(BUILD_DIR) $(DEBUG_BUILD_DIR)

format:
	clang-format -i $(shell rg --files -g "*.c" -g "*.cc" -g "*.cpp" -g "*.h" -g "*.hpp" -g "*.hxx" -g "*.vert" -g "*.frag" src)

.PHONY: all configure build run configure-debug build-debug run-debug configure-win build-win run-win clean format
