#pragma once

#include <vector>
#include <vulkan/vulkan.h>

enum class CommandBufferUsage
{
    ONE_TIME,
    SIMULTANEOUS_USE
};

class CommandBuffer
{
public:
    
    VkCommandBuffer id;

    CommandBuffer( VkDevice        device,
                   VkQueue         queue,
                   VkCommandPool   pool,
                   VkCommandBuffer cmdbuff = VK_NULL_HANDLE )
    {
        this->init( device, queue, pool, cmdbuff );
    }

    CommandBuffer() {}

    ~CommandBuffer() { this->deinit(); }

    void init( VkDevice        device,
               VkQueue         queue,
               VkCommandPool   pool,
               VkCommandBuffer cmdbuff = VK_NULL_HANDLE );

    void deinit();

    void begin( CommandBufferUsage usage = CommandBufferUsage::SIMULTANEOUS_USE );

    void end();

    void submit();

private:

    VkDevice        device      = VK_NULL_HANDLE;
    VkQueue         queue       = VK_NULL_HANDLE;
    VkCommandPool   pool        = VK_NULL_HANDLE;
    bool            initialized = false;
    bool            remoteAlloc = false;
    bool            began       = false;
    bool            ended       = false;;
};

class CommandBuffers
{
public:

    CommandBuffers( VkDevice           device,
                    VkQueue            queue,
                    VkCommandPool      pool,
                    CommandBufferUsage usage,
                    std::size_t        size );

    CommandBuffers(  );

    ~CommandBuffers(  );

    void init( VkDevice      device,
               VkQueue       queue,
               VkCommandPool pool,
               std::size_t   size );

    void deinit(  );

    void refresh( VkDevice      device,
                  VkQueue       queue,
                  VkCommandPool pool,
                  std::size_t   size );

    std::size_t size(  );

    CommandBuffer& operator[] ( std::size_t idx );

    const CommandBuffer& operator[] ( std::size_t idx ) const;

private:
    
    std::vector<CommandBuffer>   buffers;
    std::vector<VkCommandBuffer> buffpriv;
    VkDevice                     device = VK_NULL_HANDLE;
    VkQueue                      queue  = VK_NULL_HANDLE;
    VkCommandPool                pool   = VK_NULL_HANDLE;
};
