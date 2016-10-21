#ifndef __BASE_INCLUDES_HPP__
#define __BASE_INCLUDES_HPP__

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#ifdef GLM_HAS_CXX11_STL   // Hack to get GLM's
//#undef GLM_HAS_CXX11_STL  // hash support working
//#endif
#include <glm/gtx/hash.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "application.hpp"

#include <array>
#include <set>
#include <unordered_map>
#include <vector>
#include <chrono>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include "3rdparty/stb_image.h"

//#define TINYOBJLOADER_IMPLEMENTATION
//#include "3rdparty/tiny_obj_loader.h"

#endif
