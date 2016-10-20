cmake_minimum_required(VERSION 3.3)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

project(simple-renderer)

include(AppendCmakeInstallRPath)
include(FindVulkan)
include(CompileShader)

#find_package(Git REQUIRED)
find_package(Vulkan REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

# =============================================================================
#
# Build renderer
#
# =============================================================================

add_subdirectory(src)
