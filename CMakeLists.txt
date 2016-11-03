cmake_minimum_required(VERSION 3.3)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(simple-renderer)

include(AppendCmakeInstallRPath)
include(FindVulkan)
include(CompileShader)

find_package(Vulkan REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

# =============================================================================
#
# Build renderer
#
# =============================================================================

add_subdirectory(src)

# =============================================================================
#
# Install renderer
#
# =============================================================================

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/models")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/textures")


file(COPY models/chalet.obj DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/models")
file(COPY textures/chalet.jpg DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/textures")
