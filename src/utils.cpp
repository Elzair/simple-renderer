#include <cstring>
#include <stdexcept>

#include "utils.hpp"

/*
 * Formats
 */

VkFormat FindSupportedFormat( VkPhysicalDevice             physical,
                              const std::vector<VkFormat>& candidates,
                              VkImageTiling                tiling,
                              VkFormatFeatureFlags         features )
{
    for ( VkFormat format : candidates )
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties( physical, format, &props );

        if ( tiling == VK_IMAGE_TILING_LINEAR &&
             ( props.linearTilingFeatures & features ) == features )
        {
            return format;
        }
        else if ( tiling == VK_IMAGE_TILING_OPTIMAL &&
                  ( props.optimalTilingFeatures & features ) == features )
        {
            return format;
        }
    }

    throw std::runtime_error( "Failed to find supported format!" );
}

VkFormat FindDepthFormat( VkPhysicalDevice physical )
{
    return FindSupportedFormat(
        physical,
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
}

/*
 * Queue
 */

bool QueueFamilyIndices::isComplete(  )
{
    return this->graphicsFamily >= 0 && this->presentFamily >= 0;
}

QueueFamilyIndices FindQueueFamilies( VkPhysicalDevice device, VkSurfaceKHR surface )
{
    QueueFamilyIndices indices;

    uint32_t qfCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( device, &qfCount, nullptr );
    std::vector<VkQueueFamilyProperties> qfamilies( qfCount );
    vkGetPhysicalDeviceQueueFamilyProperties( device, &qfCount, qfamilies.data() );

    int i = 0;
    for ( const auto& qfamily : qfamilies )
    {
        if ( qfamily.queueCount > 0 && qfamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
        {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface, &presentSupport );
        if ( qfamily.queueCount > 0 && presentSupport )
        {
            indices.presentFamily = i;
        }

        if ( indices.isComplete() )
        {
            break;
        }

        i++;
    }

    return indices;
}

/*
 * Extensions
 */

std::vector<const char*> GetRequiredExtensions( bool validate )
{
    std::vector<const char*> extensions;

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

    for ( auto i = 0; i < glfwExtensionCount; i++ )
    {
        extensions.push_back( glfwExtensions[ i ] );
    }

    if ( validate )
    {
        extensions.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
    }

    return extensions;
}

bool CheckDeviceExtensionSupport(
    VkPhysicalDevice               device,
    const std::vector<const char*> requiredDeviceExtensions
    )
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );
    std::vector<VkExtensionProperties> availableExtensions( extensionCount );
    vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions.data() );

    // Ensure all required extensions are available on device
    for ( const auto& requiredExt : requiredDeviceExtensions )
    {
        bool found = false;

        for ( const auto& ext : availableExtensions )
        {
            if ( std::strcmp( ext.extensionName, requiredExt ) == 0 )
            {
                found = true;
            }
        }

        if ( !found )
        {
            return false;
        }
    }

    return true;
}

/*
 * Swap Chain
 */

SwapChainSupportDetails QuerySwapChainSupport( VkPhysicalDevice physical,
                                               VkSurfaceKHR     surface )
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physical,
                                               surface,
                                               &details.capabilities );

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR( physical,
                                          surface,
                                          &formatCount,
                                          nullptr );
    if ( formatCount > 0 )
    {
        details.formats.resize( formatCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( physical,
                                              surface,
                                              &formatCount,
                                              details.formats.data() );
    }

    uint32_t presentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR( physical,
                                               surface,
                                               &presentModesCount,
                                               nullptr );
    if ( presentModesCount > 0 )
    {
        details.presentModes.resize( presentModesCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( physical,
                                                   surface,
                                                   &presentModesCount,
                                                   details.presentModes.data() );
    }

    return details;
}

/*
 * Physical Device Selection
 */

bool IsDeviceSuitable(
    VkPhysicalDevice               physical,
    VkSurfaceKHR                   surface,
    const std::vector<const char*> requiredExtensions
    )
{
    auto indices = FindQueueFamilies( physical, surface );

    bool requiredExtensionsSupported = CheckDeviceExtensionSupport(
        physical,
        requiredExtensions
        );

    bool adequateSwapChain = false;
    if ( requiredExtensionsSupported )
    {
        auto swapchainSupport = QuerySwapChainSupport( physical, surface );
        adequateSwapChain = !swapchainSupport.formats.empty() &&
            !swapchainSupport.presentModes.empty();
    }

    return indices.isComplete() &&
        requiredExtensionsSupported &&
        adequateSwapChain;
}

VkPhysicalDevice PickPhysicalDevice(
    VkInstance                     instance,
    VkSurfaceKHR                   surface,
    const std::vector<const char*> requiredExtensions
    )
{
    uint32_t devCount = 0;
    vkEnumeratePhysicalDevices( instance, &devCount, nullptr);
    if ( devCount == 0 )
    {
        throw std::runtime_error( "Failed to find GPUs with Vulkan support!" );
    }
    std::vector<VkPhysicalDevice> physicalDevices( devCount );
    vkEnumeratePhysicalDevices( instance, &devCount, physicalDevices.data() );

    for ( const auto& physical : physicalDevices )
    {
        if ( IsDeviceSuitable( physical, surface, requiredExtensions ) )
        {
            return physical;
        }
    }

    throw std::runtime_error( "Failed to find a suitable GPU!" );
}

/*
 * Debug Report
 */

VkResult CreateDebugReportCallbackEXT(
    VkInstance                                instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks*              pAllocator,
    VkDebugReportCallbackEXT*                 pCallback
    )
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT)
        vkGetInstanceProcAddr( instance, "vkCreateDebugReportCallbackEXT" );

    if ( func != nullptr )
    {
        return func( instance, pCreateInfo, pAllocator, pCallback );
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugReportCallbackEXT(
    VkInstance                   instance,
    VkDebugReportCallbackEXT     callback,
    const VkAllocationCallbacks* pAllocator
    )
{
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)
        vkGetInstanceProcAddr( instance, "vkDestroyDebugReportCallbackEXT" );
    if ( func != nullptr )
    {
        func( instance, callback, pAllocator );
    }
}

/*
 * Memory
 */

uint32_t FindMemoryType( VkPhysicalDevice      physical,
                         uint32_t              typeFilter,
                         VkMemoryPropertyFlags props )
{
    VkPhysicalDeviceMemoryProperties memprops;
    vkGetPhysicalDeviceMemoryProperties( physical, &memprops );

    for ( uint32_t i = 0; i < memprops.memoryTypeCount; i++ )
    {
        if ( ( typeFilter & ( 1 << i ) ) &&
             ( memprops.memoryTypes[i].propertyFlags & props ) == props )
        {
            return i;
        }
    }

    throw std::runtime_error( "Failed to find suitable memory type!" );
}
