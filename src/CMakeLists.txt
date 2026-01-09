include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

find_package(OpenGL REQUIRED)

add_library(imgui_lib
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_lib PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_lib PUBLIC glfw OpenGL::GL)

add_library(engine
  engine/core/Color4f.h
  engine/core/InputState.h
  engine/core/IRenderer.h
  engine/core/IWindow.h
  engine/core/Logger.cpp
  engine/core/Logger.h
  engine/scene/FrameContext.h
  engine/scene/IScene.h
  engine/scene/SceneManager.cpp
  engine/scene/SceneManager.h
  engine/render/PixelRenderer.cpp
  engine/render/PixelRenderer.h
  engine/render/opengl/GlPresenter.cpp
  engine/render/opengl/GlPresenter.h
  engine/platform/glfw/GlfwWindow.cpp
  engine/platform/glfw/GlfwWindow.h
  engine/ui/EditorUi.cpp
  engine/ui/EditorUi.h
  engine/ui/ImGuiLayer.cpp
  engine/ui/ImGuiLayer.h
)

target_include_directories(engine PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/engine/math/sgm/public
)
target_link_libraries(engine PUBLIC glfw OpenGL::GL imgui_lib)

if(APPLE)
  target_compile_definitions(engine PUBLIC GL_SILENCE_DEPRECATION)
endif()

file(GLOB APP_SCENE_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/app/scenes/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/app/scenes/*.h
)

add_executable(sandbox
  app/main.cpp
  ${APP_SCENE_SOURCES}
)

target_link_libraries(sandbox PRIVATE engine)
