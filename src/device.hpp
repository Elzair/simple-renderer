#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class Device
{
public:
    VkDevice         id = VK_NULL_HANDLE;

    int              graphicsQueueIdx = -1;
    int              presentQueueIdx  = -1;

    VkQueue          graphicsQueue = VK_NULL_HANDLE;
    VkQueue          presentQueue  = VK_NULL_HANDLE;

    Device( VkPhysicalDevice               physical,
            VkSurfaceKHR                   surface,
            const std::vector<const char*> extensions,
            const std::vector<const char*> validationLayers )
    {
        this->init( physical, surface, extensions, validationLayers );
    }

    Device() {}

    ~Device() { this->deinit(); }

    void init( VkPhysicalDevice               physical,
               VkSurfaceKHR                   surface,
               const std::vector<const char*> extensions,
               const std::vector<const char*> validationLayers );

    void deinit();
};
