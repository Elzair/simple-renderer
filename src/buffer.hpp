#pragma once

#include "commandbuffer.hpp"
#include "device.hpp"
#include "utils.hpp"

class CommandPool;

enum class BufferUsage
{
    VERTEX,
    INDEX,
    UNIFORM
};

class Buffer
{
    friend class DescriptorSet;

public:

    VkBuffer       id;
    VkDeviceMemory memory;

    Buffer( Device*      device,
            VkQueue      queue,
            CommandPool* commandPool,
            VkDeviceSize size,
            BufferUsage  usage )
    {
        this->init( device, queue, commandPool, size, usage );
    }

    Buffer() {}

    ~Buffer() { this->deinit(); }

    void init( Device*      device,
               VkQueue      queue,
               CommandPool* commandPool,
               VkDeviceSize size,
               BufferUsage  usage );

    void deinit();

    void copy( void*       data     = nullptr,
               bool        toDevice = true,
               std::size_t len      = UINT32_MAX );

private:

    VkBuffer         staging       = VK_NULL_HANDLE;
    VkDeviceMemory   stagingMemory = VK_NULL_HANDLE;

    Device*          device        = nullptr;
    VkQueue          queue         = VK_NULL_HANDLE;
    CommandPool*     commandPool   = nullptr;
    VkDeviceSize     size          = 0;
    bool             initialized   = false;

    VkBuffer createBuffer( VkBufferUsageFlags usage );

    VkDeviceMemory allocateMemory( VkBuffer              buffer,
                                   VkMemoryPropertyFlags props );
};
