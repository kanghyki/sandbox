cmake_minimum_required(VERSION 3.14)

project(
  sgm
  VERSION 1.0
  LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME}
INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/public
)
