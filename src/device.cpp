#include <set>
#include <stdexcept>
#include "utils.hpp"
#include "device.hpp"

Device::Device( VkPhysicalDevice               physical,
                VkSurfaceKHR                   surface,
                const std::vector<const char*> extensions,
                const std::vector<const char*> validationLayers )
{
    this->init( physical, surface, extensions, validationLayers );
}

Device::Device(  ) { }

Device::~Device(  )
{
    this->deinit();
}

void Device::init( VkPhysicalDevice               physical,
                   VkSurfaceKHR                   surface,
                   const std::vector<const char*> extensions,
                   const std::vector<const char*> validationLayers )
{
    // Create queues for both the graphics and presentation families
    QueueFamilyIndices indices = FindQueueFamilies( physical,
                                                    surface );
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = {
        indices.graphicsFamily,
        indices.presentFamily
    };
    this->graphicsQueueIdx = indices.graphicsFamily;
    this->presentQueueIdx  = indices.presentFamily;
        
    float queuePriority = 1.0f;

    for ( int queueFamily : uniqueQueueFamilies )
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount       = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back( queueCreateInfo );
    }

    VkPhysicalDeviceFeatures devFeatures = {};

    // Create struct used to create a logical device
    VkDeviceCreateInfo devCreateInfo = {};
    devCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devCreateInfo.pQueueCreateInfos       = queueCreateInfos.data();
    devCreateInfo.queueCreateInfoCount    = (uint32_t)queueCreateInfos.size();
    devCreateInfo.pEnabledFeatures        = &devFeatures;
    devCreateInfo.enabledExtensionCount   = extensions.size();
    devCreateInfo.ppEnabledExtensionNames = extensions.data();
    if ( validationLayers.size() > 0 )
    {
        devCreateInfo.enabledLayerCount   = validationLayers.size();
        devCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        devCreateInfo.enabledLayerCount = 0;
    }

    if ( vkCreateDevice( physical, &devCreateInfo,
                         nullptr, &this->id ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create logical device!" );
    }

    // Retrieve handles for graphics and presentation queues
    vkGetDeviceQueue( this->id, this->graphicsQueueIdx,
                      0, &this->graphicsQueue);
    vkGetDeviceQueue( this->id, this->presentQueueIdx,
                      0, &this->presentQueue );
}

void Device::deinit(  )
{
    if ( this->id != VK_NULL_HANDLE )
    {
        vkDestroyDevice( this->id, nullptr );
    }
}
