BUILD_DIR ?= build
DEBUG_BUILD_DIR ?= build-debug

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
	cmake --build $(DEBUG_BUILD_DIR)

run-debug: build-debug
	./$(DEBUG_BUILD_DIR)/src/sandbox

clean:
	rm -rf $(BUILD_DIR) $(DEBUG_BUILD_DIR)

format:
	clang-format -i $(shell rg --files -g "*.c" -g "*.cc" -g "*.cpp" -g "*.h" -g "*.hpp" -g "*.hxx" -g "*.vert" -g "*.frag" src)

.PHONY: all configure build run configure-debug build-debug run-debug clean format
