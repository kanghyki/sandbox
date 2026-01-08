BUILD_DIR ?= build

all: build

generate_scenes:
	python3 tools/gen_scene_registry.py

configure: generate_scenes
	cmake -S . -B $(BUILD_DIR) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/src/sandbox

clean:
	rm -rf $(BUILD_DIR)

format:
	clang-format -i $(shell rg --files -g "*.c" -g "*.cc" -g "*.cpp" -g "*.h" -g "*.hpp" -g "*.hxx" -g "*.vert" -g "*.frag" src)

.PHONY: all configure build run clean format
