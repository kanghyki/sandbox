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

find_package(OpenGL REQUIRED)

add_library(engine
  engine/core/Color.h
  engine/core/InputState.h
  engine/core/IRenderer.h
  engine/core/IWindow.h
  engine/render/PixelRenderer.cpp
  engine/render/PixelRenderer.h
  engine/render/opengl/GlPresenter.cpp
  engine/render/opengl/GlPresenter.h
  engine/platform/glfw/GlfwWindow.cpp
  engine/platform/glfw/GlfwWindow.h
)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine PUBLIC glfw OpenGL::GL)

if(APPLE)
  target_compile_definitions(engine PUBLIC GL_SILENCE_DEPRECATION)
endif()

add_executable(sandbox
  app/main.cpp
)

target_link_libraries(sandbox PRIVATE engine)
