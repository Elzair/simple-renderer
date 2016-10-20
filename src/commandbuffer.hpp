#ifndef __COMMANDBUFFER_HPP__
#define __COMMANDBUFFER_HPP__

#include "base-includes.hpp"

enum class CommandBufferUsage
{
    ONE_TIME,
    SIMULTANEOUS_USE
};

class CommandBuffer
{
public:
    
    VkCommandBuffer id;

    CommandBuffer(  VkDevice        device,
                    VkQueue         queue,
                    VkCommandPool   pool,
                    VkCommandBuffer cmdbuff = VK_NULL_HANDLE )
    {
        this->init( device, queue, pool, cmdbuff );
    }

    CommandBuffer(  ) { }

    ~CommandBuffer(  )
    {
        this->deinit();
    }

    void init( VkDevice        device,
               VkQueue         queue,
               VkCommandPool   pool,
               VkCommandBuffer cmdbuff = VK_NULL_HANDLE )
    {
        if ( device == VK_NULL_HANDLE ||
             queue == VK_NULL_HANDLE ||
             pool == VK_NULL_HANDLE )
        {
            throw std::invalid_argument( "Attempt to initialize CommandBuffer with invalid arguments!" );
        }

        this->device    = device;
        this->queue     = queue;
        this->pool      = pool;

        if ( cmdbuff == VK_NULL_HANDLE )
        {
            VkCommandBufferAllocateInfo allocInfo = {};
            allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool        = this->pool;
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers( this->device, &allocInfo, &this->id );

            this->remoteAlloc = false;
        }
        else
        {
            this->id          = cmdbuff;
            this->remoteAlloc = true;
        }

        this->initialized = true;
    }

    void deinit(  )
    {
        if ( this->began && !this->ended )
        {
            this->end();
        }

        if ( !this->remoteAlloc )
        {
            vkQueueWaitIdle( this->queue );

            vkFreeCommandBuffers( this->device, this->pool, 1, &this->id );
        }

        this->initialized = false;
        this->remoteAlloc = false;
        this->began       = false;
        this->ended       = false;
    }

    void begin( CommandBufferUsage usage = CommandBufferUsage::SIMULTANEOUS_USE )
    {
        if ( !this->initialized )
        {
            throw std::runtime_error( "Tried to began an uninitialized command buffer!" );
        }

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        switch( usage )
        {
        case CommandBufferUsage::ONE_TIME:
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            break;
        case CommandBufferUsage::SIMULTANEOUS_USE:
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            break;
        }

        vkBeginCommandBuffer( this->id, &beginInfo );

        this->began = true;
    }

    void end(  )
    {
        if ( !this->initialized || !this->began )
        {
            throw std::runtime_error( "Cannot end an uninitialized/empty command buffer!" );
        }
        else if ( this->ended )
        {
            return;
        }

        if ( vkEndCommandBuffer( this->id ) != VK_SUCCESS )
        {
            throw std::runtime_error( "Failed to record command buffer!" );
        }

        this->ended = true;
    }

    void submit(  )
    {
        if ( !this->initialized || !this->began )
        {
            throw std::runtime_error( "Cannot submit an uninitialized/empty command buffer!" );
        }

        if ( !this->ended )
        {
            this->end();
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &this->id;

        vkQueueSubmit( this->queue, 1, &submitInfo, VK_NULL_HANDLE );
    }

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
                    std::size_t        size )
    {
        this->init( device, queue, pool, size );
    }

    CommandBuffers(  ) { }

    ~CommandBuffers(  )
    {
        this->deinit();
    }

    void init( VkDevice      device,
               VkQueue       queue,
               VkCommandPool pool,
               std::size_t   size )
    {
        this->device = device;
        this->queue  = queue;
        this->pool   = pool;
        
        // Free old command buffers (if called from recreateSwapChain())
        this->buffpriv.resize( size );
        this->buffers.resize( size );

        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool        = this->pool;
        allocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = (uint32_t)this->buffpriv.size();

        if ( vkAllocateCommandBuffers( this->device,
                                       &allocateInfo,
                                       this->buffpriv.data() ) != VK_SUCCESS )
        {
            throw std::runtime_error( "Failed to allocate command buffers!" );
        }

        for ( std::size_t i = 0; i < this->buffers.size(); i++ )
        {
            this->buffers[i].init( this->device,
                                   this->queue,
                                   this->pool,
                                   this->buffpriv[i] );
        }
    }

    void deinit(  )
    {
        if ( this->buffers.size() > 0 )
        {
            vkFreeCommandBuffers( this->device,
                                  this->pool,
                                  this->buffpriv.size(),
                                  this->buffpriv.data() );
            this->buffers.clear();
        }
    }

    void refresh( VkDevice      device,
                  VkQueue       queue,
                  VkCommandPool pool,
                  size_t        size )
    {
        this->deinit(  );
        this->init( device, queue, pool, size );
    }

    std::size_t size(  )
    {
        return this->buffers.size();
    }

    CommandBuffer& operator[] ( std::size_t idx )
    {
        return this->buffers[idx];
    }

    const CommandBuffer& operator[] ( std::size_t idx ) const
    {
        return this->buffers[idx];
    }

private:
    
    std::vector<CommandBuffer>   buffers;
    std::vector<VkCommandBuffer> buffpriv;
    VkDevice                     device = VK_NULL_HANDLE;
    VkQueue                      queue  = VK_NULL_HANDLE;
    VkCommandPool                pool   = VK_NULL_HANDLE;
};

#endif
