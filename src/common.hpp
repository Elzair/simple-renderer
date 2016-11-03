#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

const int WIDTH  = 800;
const int HEIGHT = 600;

const std::string MODEL_PATH   = "models/chalet.obj";
const std::string TEXTURE_PATH = "textures/chalet.jpg";

static inline void CheckResult( VkResult    res,
                                const char* file,
                                const char* func,
                                int         line )
{
    if ( res != VK_SUCCESS )
    {
        std::cerr << "Error in " << file << " " << func << " " << line << ": VkResult is \"";
        switch( res )
        {
        case VK_NOT_READY:
            std::cerr << "NOT_READY";
            break;
        case VK_TIMEOUT:
            std::cerr << "TIMEOUT";
            break;
        case VK_EVENT_SET:
            std::cerr << "EVENT_SET";
            break;
        case VK_EVENT_RESET:
            std::cerr << "EVENT_RESET";
            break;
        case VK_INCOMPLETE:
            std::cerr << "INCOMPLETE";
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            std::cerr << "ERROR_OUT_OF_HOST_MEMORY";
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            std::cerr << "ERROR_OUT_OF_DEVICE_MEMORY";
            break;
        case VK_ERROR_INITIALIZATION_FAILED:
            std::cerr << "ERROR_INITIALIZATION_FAILED";
            break;
        case VK_ERROR_DEVICE_LOST:
            std::cerr << "ERROR_DEVICE_LOST";
            break;
        case VK_ERROR_MEMORY_MAP_FAILED:
            std::cerr << "ERROR_MEMORY_MAP_FAILED";
            break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            std::cerr << "ERROR_LAYER_NOT_PRESENT";
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            std::cerr << "ERROR_EXTENSION_NOT_PRESENT";
            break;
        case VK_ERROR_FEATURE_NOT_PRESENT:
            std::cerr << "ERROR_FEATURE_NOT_PRESENT";
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            std::cerr << "ERROR_INCOMPATIBLE_DRIVER";
            break;
        case VK_ERROR_TOO_MANY_OBJECTS:
            std::cerr << "ERROR_TOO_MANY_OBJECTS";
            break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            std::cerr << "ERROR_FORMAT_NOT_SUPPORTED";
            break;
        case VK_ERROR_SURFACE_LOST_KHR:
            std::cerr << "ERROR_SURFACE_LOST_KHR";
            break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            std::cerr << "ERROR_NATIVE_WINDOW_IN_USE_KHR";
            break;
        case VK_SUBOPTIMAL_KHR:
            std::cerr << "SUBOPTIMAL_KHR";
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            std::cerr << "ERROR_OUT_OF_DATE_KHR";
            break;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            std::cerr << "ERROR_INCOMPATIBLE_DISPLAY_KHR";
            break;
        case VK_ERROR_VALIDATION_FAILED_EXT:
            std::cerr << "ERROR_VALIDATION_FAILED_EXT";
            break;
        case VK_ERROR_INVALID_SHADER_NV:
            std::cerr << "ERROR_INVALID_SHADER_NV";
            break;
        default:
            std::cerr << "Unknown Error";
            break;
        }
        std::cerr << "\"" << std::endl;
    }

    assert( res == VK_SUCCESS );
}

#ifdef NDEBUG

const bool enableValidationLayers = false;

const std::vector<const char*> requiredValidationLayers = {};

#define VK_CHECK_RESULT( res ) assert( ( res ) == VK_SUCCESS )

#else

const bool enableValidationLayers = true;

const std::vector<const char*> requiredValidationLayers = {
  "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> requiredDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#define VK_CHECK_RESULT( res ) CheckResult( res, __FILE__, __func__, __LINE__ )

#endif

#endif
