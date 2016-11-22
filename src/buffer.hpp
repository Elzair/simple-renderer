#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__

//#include "base-includes.hpp"
#include "commandbuffer.hpp"
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
            VkDevice         device,
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
               VkDevice         device,
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
    VkDevice         device        = VK_NULL_HANDLE;
    VkPhysicalDevice physical      = VK_NULL_HANDLE;
    VkQueue          queue         = VK_NULL_HANDLE;
    VkCommandPool    commandPool   = VK_NULL_HANDLE;
    VkDeviceSize     size          = 0;
    bool             initialized   = false;

    VkBuffer createBuffer( VkBufferUsageFlags usage );

    VkDeviceMemory allocateMemory( VkBuffer              buffer,
                                   VkMemoryPropertyFlags props );
};

#endif
