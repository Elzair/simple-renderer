#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

//#include "base-includes.hpp"
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
            const std::vector<const char*> validationLayers );

    Device(  );

    ~Device(  );

    void init( VkPhysicalDevice               physical,
               VkSurfaceKHR                   surface,
               const std::vector<const char*> extensions,
               const std::vector<const char*> validationLayers );

    void deinit(  );
};

#endif
