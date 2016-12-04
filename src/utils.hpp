#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

/*
 * File IO
 */

std::vector<uint8_t> ReadFile( const std::string& filename );

/*
 * Formats
 */

VkFormat FindSupportedFormat( VkPhysicalDevice             physical,
                              const std::vector<VkFormat>& candidates,
                              VkImageTiling                tiling,
                              VkFormatFeatureFlags         features );

VkFormat FindDepthFormat( VkPhysicalDevice physical );

/*
 * Queue
 */

struct QueueFamilyIndices
{
    int graphicsFamily = -1;
    int presentFamily  = -1;

    bool isComplete(  );
};

QueueFamilyIndices FindQueueFamilies( VkPhysicalDevice device,
                                      VkSurfaceKHR surface );

/*
 * Extensions
 */

std::vector<const char*> GetRequiredExtensions( bool validate );

bool CheckDeviceExtensionSupport(
    VkPhysicalDevice               device,
    const std::vector<const char*> requiredDeviceExtensions
    );

/*
 * Swap Chain
 */

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

SwapChainSupportDetails QuerySwapChainSupport( VkPhysicalDevice physical,
                                               VkSurfaceKHR     surface );

/*
 * Physical Device Selection
 */

bool IsDeviceSuitable(
    VkPhysicalDevice               physical,
    VkSurfaceKHR                   surface,
    const std::vector<const char*> requiredExtensions
    );

VkPhysicalDevice PickPhysicalDevice(
    VkInstance                     instance,
    VkSurfaceKHR                   surface,
    const std::vector<const char*> requiredExtensions
    );

/*
 * Debug Report
 */

#if defined( DEBUG_BUILD )
VkResult CreateDebugReportCallbackEXT(
    VkInstance                                instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks*              pAllocator,
    VkDebugReportCallbackEXT*                 pCallback
    );

void DestroyDebugReportCallbackEXT(
    VkInstance                   instance,
    VkDebugReportCallbackEXT     callback,
    const VkAllocationCallbacks* pAllocator
    );
#endif

/*
 * Memory
 */

uint32_t FindMemoryType( VkPhysicalDevice      physical,
                         uint32_t              typeFilter,
                         VkMemoryPropertyFlags props );
