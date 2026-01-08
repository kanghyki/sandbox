#!/usr/bin/env python3
import os
import re


def main():
    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    scenes_dir = os.path.join(repo_root, "src", "app", "scenes")
    if not os.path.isdir(scenes_dir):
        print("Scene directory not found.")
        return 1

    headers = []
    for name in os.listdir(scenes_dir):
        if not name.endswith("Scene.h"):
            continue
        if name == "SceneRegistry.h":
            continue
        headers.append(name)
    headers.sort()

    classes = [re.sub(r"\.h$", "", h) for h in headers]

    header_path = os.path.join(scenes_dir, "SceneRegistry.h")
    cpp_path = os.path.join(scenes_dir, "SceneRegistry.cpp")

    header = """#pragma once

class SceneManager;

void RegisterScenes(SceneManager& scenes);
"""

    includes = "\n".join([f'#include "app/scenes/{h}"' for h in headers])
    registrations = "\n".join(
        [f"    scenes.AddScene(std::make_unique<{cls}>());" for cls in classes]
    )
    cpp = f"""#include "app/scenes/SceneRegistry.h"

#include "engine/scene/SceneManager.h"

{includes}

#include <memory>

void RegisterScenes(SceneManager& scenes) {{
{registrations}
}}
"""

    with open(header_path, "w", encoding="utf-8") as f:
        f.write(header)
    with open(cpp_path, "w", encoding="utf-8") as f:
        f.write(cpp)

    print(f"Registered {len(classes)} scene(s).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
