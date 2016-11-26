#pragma once

#include "commandbuffer.hpp"
#include "device.hpp"
#include "utils.hpp"

enum BufferUsage
{
    VERTEX,
    INDEX,
    UNIFORM
};

class Buffer
{
public:

    VkBuffer       id;
    VkDeviceMemory memory;

    Buffer( VkPhysicalDevice physical,
            Device*          device,
            VkQueue          queue,
            VkCommandPool    commandPool,
            VkDeviceSize     size,
            BufferUsage      usage )
    {
        this->init( physical, device, queue, commandPool, size, usage );
    }

    Buffer() {}

    ~Buffer() { this->deinit(); }

    void init( VkPhysicalDevice physical,
               Device*          device,
               VkQueue          queue,
               VkCommandPool    commandPool,
               VkDeviceSize     size,
               BufferUsage      usage );

    void deinit();

    void copy( void*  data     = nullptr,
               bool   toDevice = true,
               size_t len      = UINT32_MAX );

private:

    VkBuffer         staging       = VK_NULL_HANDLE;
    VkDeviceMemory   stagingMemory = VK_NULL_HANDLE;
    Device*          device        = nullptr;

    VkPhysicalDevice physical      = VK_NULL_HANDLE;
    VkQueue          queue         = VK_NULL_HANDLE;
    VkCommandPool    commandPool   = VK_NULL_HANDLE;
    VkDeviceSize     size          = 0;
    bool             initialized   = false;

    VkBuffer createBuffer( VkBufferUsageFlags usage );

    VkDeviceMemory allocateMemory( VkBuffer              buffer,
                                   VkMemoryPropertyFlags props );
};
