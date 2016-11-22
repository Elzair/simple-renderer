#include <cstring>
#include "common.hpp"
#include "buffer.hpp"

void Buffer::init( VkPhysicalDevice physical,
                   VkDevice         device,
                   VkQueue          queue,
                   VkCommandPool    commandPool,
                   VkDeviceSize     size,
                   BufferUsage      usage )
{
    this->device      = device;
    this->physical    = physical;
    this->queue       = queue;
    this->commandPool = commandPool;
    this->size        = size;

    // Create staging buffer
        
    VkBufferUsageFlags    uflags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags pflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    this->staging       = this->createBuffer( uflags );
    this->stagingMemory = this->allocateMemory( staging, pflags );

    //  Create device buffer
        
    uflags = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    switch( usage )
    {
    case BufferUsage::VERTEX:
        uflags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
    case BufferUsage::INDEX:
        uflags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        break;
    case BufferUsage::UNIFORM:
        uflags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        break;
    }
    pflags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    this->id     = this->createBuffer( uflags );
    this->memory = this->allocateMemory( this->id, pflags );

    this->initialized = true;
}

void Buffer::deinit(  )
{
    if ( this->memory != VK_NULL_HANDLE )
    {
        vkFreeMemory( this->device, this->memory, nullptr );
    }
    if ( this->id != VK_NULL_HANDLE )
    {
        vkDestroyBuffer( this->device, this->id, nullptr );
    }
    if ( this->stagingMemory != VK_NULL_HANDLE )
    {
        vkFreeMemory( this->device, this->stagingMemory, nullptr );
    }
    if ( this->staging != VK_NULL_HANDLE )
    {
        vkDestroyBuffer( this->device, this->staging, nullptr );
    }
}

void Buffer::copy( void*       data,
                   bool        toDevice,
                   std::size_t len )
{
    assert( this->initialized );

    len = ( len > this->size ) ? this->size : len;

    if ( data != nullptr )
    {
        void*        mem  = nullptr;
        VkDeviceSize size = (VkDeviceSize)this->size;
            
        vkMapMemory( this->device,
                     this->stagingMemory,
                     0,
                     size,
                     0,
                     &mem );
        std::memcpy( mem, data, size );
        vkUnmapMemory( this->device, this->stagingMemory );
    }

    if ( toDevice )
    {
        CommandBuffer commandBuffer( this->device,
                                     this->queue,
                                     this->commandPool );
        commandBuffer.begin( CommandBufferUsage::ONE_TIME );
            
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size      = size;
        vkCmdCopyBuffer( commandBuffer.id,
                         this->staging,
                         this->id,
                         1,
                         &copyRegion );

        commandBuffer.end();
        commandBuffer.submit();
    }
}

VkBuffer Buffer::createBuffer( VkBufferUsageFlags usage )
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = this->size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer = VK_NULL_HANDLE;
    VK_CHECK_RESULT( vkCreateBuffer( this->device,
                                     &bufferInfo,
                                     nullptr,
                                     &buffer ) );

    return buffer;
}

VkDeviceMemory Buffer::allocateMemory( VkBuffer              buffer,
                                       VkMemoryPropertyFlags props )
{
    VkMemoryRequirements memreqs;
    vkGetBufferMemoryRequirements( this->device,
                                   buffer,
                                   &memreqs );

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memreqs.size;
    allocInfo.memoryTypeIndex = FindMemoryType( this->physical,
                                                memreqs.memoryTypeBits,
                                                props );

    VkDeviceMemory memory2 = VK_NULL_HANDLE;
    VK_CHECK_RESULT( vkAllocateMemory( this->device,
                                       &allocInfo,
                                       nullptr,
                                       &memory2 ) );

    vkBindBufferMemory( this->device, buffer, memory2, 0 );

    return memory2;
}
