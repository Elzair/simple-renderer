#include "common.hpp"
#include "commandbuffer.hpp"

CommandBuffer::CommandBuffer( VkDevice        device,
                              VkQueue         queue,
                              VkCommandPool   pool,
                              VkCommandBuffer cmdbuff )
{
    this->init( device, queue, pool, cmdbuff );
}

CommandBuffer::CommandBuffer(  ) { }

CommandBuffer::~CommandBuffer(  )
{
    this->deinit();
}

void CommandBuffer::init( VkDevice        device,
                          VkQueue         queue,
                          VkCommandPool   pool,
                          VkCommandBuffer cmdbuff )
{
    assert( ( device != VK_NULL_HANDLE ) &&
            ( queue != VK_NULL_HANDLE ) &&
            ( pool != VK_NULL_HANDLE ) );

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

void CommandBuffer::deinit(  )
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

void CommandBuffer::begin( CommandBufferUsage usage )
{
    assert( this->initialized );

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

void CommandBuffer::end(  )
{
    assert( this->initialized && this->began );

    if ( this->ended )
    {
        return;
    }

    VK_CHECK_RESULT( vkEndCommandBuffer( this->id ) );

    this->ended = true;
}

void CommandBuffer::submit(  )
{
    assert( this->initialized && this->began );

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

CommandBuffers::CommandBuffers( VkDevice           device,
                                VkQueue            queue,
                                VkCommandPool      pool,
                                CommandBufferUsage usage,
                                std::size_t        size )
{
    this->init( device, queue, pool, size );
}

CommandBuffers::CommandBuffers(  ) { }

CommandBuffers::~CommandBuffers(  )
{
    this->deinit();
}

void CommandBuffers::init( VkDevice      device,
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

    VK_CHECK_RESULT( vkAllocateCommandBuffers( this->device,
                                               &allocateInfo,
                                               this->buffpriv.data() ) );

    for ( std::size_t i = 0; i < this->buffers.size(); i++ )
    {
        this->buffers[i].init( this->device,
                               this->queue,
                               this->pool,
                               this->buffpriv[i] );
    }
}

void CommandBuffers::deinit(  )
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

void CommandBuffers::refresh( VkDevice      device,
                              VkQueue       queue,
                              VkCommandPool pool,
                              std::size_t   size )
{
    this->deinit(  );
    this->init( device, queue, pool, size );
}

std::size_t CommandBuffers::size(  )
{
    return this->buffers.size();
}

CommandBuffer& CommandBuffers::operator[] ( std::size_t idx )
{
    return this->buffers[idx];
}

const CommandBuffer& CommandBuffers::operator[] ( std::size_t idx ) const
{
    return this->buffers[idx];
}
