BUILD_DIR ?= build
DEBUG_BUILD_DIR ?= build-debug
WEB_BUILD_DIR ?= build-web
EMSDK_PATH ?= $(HOME)/emsdk
EMSDK_ENV ?= $(EMSDK_PATH)/emsdk_env.sh

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

configure-web: generate_scenes
	bash -lc "source $(EMSDK_ENV) && emcmake cmake -S . -B $(WEB_BUILD_DIR) -DCMAKE_BUILD_TYPE=Release"

build-web: configure-web
	cmake --build $(WEB_BUILD_DIR) -j

run-web: build-web
	cd dist && python3 -m http.server 8000

clean:
	rm -rf $(BUILD_DIR) $(DEBUG_BUILD_DIR) $(WEB_BUILD_DIR)

format:
	clang-format -i $(shell rg --files -g "*.c" -g "*.cc" -g "*.cpp" -g "*.h" -g "*.hpp" -g "*.hxx" -g "*.vert" -g "*.frag" src)

.PHONY: all configure build run configure-debug build-debug run-debug configure-web build-web run-web clean format
