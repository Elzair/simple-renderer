#include "common.hpp"
#include "commandbuffer.hpp"

void CommandPool::init( Device*  device,
                        VkQueue  queue,
                        uint32_t queueIdx )
{
    this->device = device;
    this->queue  = queue;
    
    VkCommandPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.flags            = 0;
    poolCreateInfo.queueFamilyIndex = this->device->graphicsQueueIdx; 

    VK_CHECK_RESULT( this->device->createCommandPool( &poolCreateInfo,
                                                      &this->id ) );
}

void CommandPool::deinit()
{
    this->device->destroyCommandPool( this->id );
}

CommandBuffer CommandPool::allocateCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = this->id;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdbuf;
    this->device->allocateCommandBuffers( &allocInfo, &cmdbuf );
   
    CommandBuffer buffer( this->device, this->queue, this, cmdbuf );

    return buffer;
}

void CommandPool::reset()
{
    this->device->resetCommandPool( this->id,
                                    VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT );
}

void CommandPool::allocateCommandBuffer2( VkCommandBuffer* cmdbuf )
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = this->id;
    allocInfo.commandBufferCount = 1;

    this->device->allocateCommandBuffers( &allocInfo, cmdbuf );
}

void CommandPool::freeCommandBuffer( VkCommandBuffer* commandBuffer )
{
    this->device->freeCommandBuffers( this->id, 1, commandBuffer );
}

void CommandBuffer::init( Device*      device,
                          VkQueue      queue,
                          CommandPool* pool )
{
    this->device = device;
    this->queue  = queue;
    this->pool   = pool;

    this->pool->allocateCommandBuffer2( &this->id );
}

void CommandBuffer::deinit()
{
    this->id         = VK_NULL_HANDLE;
    this->pool       = nullptr;
    this->queue      = VK_NULL_HANDLE;
    this->device     = nullptr;
    this->began      = false;
    this->renderPass = false;
    this->ended      = false;
}

// Basic Commands

void CommandBuffer::begin( CommandBufferUsage usage )
{
    assert( !this->began );
    
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

void CommandBuffer::end()
{
    assert( this->began && !this->ended );

    VK_CHECK_RESULT( vkEndCommandBuffer( this->id ) );

    this->ended = true;
}

// RenderPass Commands
void CommandBuffer::beginRenderPass( RenderPass&              renderPass,
                                     VkFramebuffer              framebuffer,
                                     VkRect2D                   renderArea,
                                     std::vector<VkClearValue>& clearValues,
                                     VkSubpassContents        contents )
{
    assert( this->began && !this->ended && !this->renderPass );

    VkRenderPassBeginInfo createInfo = {};
    createInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    createInfo.renderPass      = renderPass.renderPass;
    createInfo.framebuffer     = framebuffer;
    createInfo.renderArea      = renderArea;
    createInfo.clearValueCount = clearValues.size();
    createInfo.pClearValues    = clearValues.data();

    vkCmdBeginRenderPass( this->id, &createInfo, contents );

    this->renderPass = true;
}

void CommandBuffer::nextSubpass( VkSubpassContents contents )
{
    assert( this->began && !this->ended && this->renderPass );
    
    vkCmdNextSubpass( this->id, contents );
}

void CommandBuffer::endRenderPass()
{
    assert( this->began && !this->ended && this->renderPass );

    vkCmdEndRenderPass( this->id );

    this->renderPass = false;
}

void CommandBuffer::executeCommands( uint32_t               commandBufferCount,
                                     const VkCommandBuffer* pCommandBuffers )
{
    assert( this->began );

    vkCmdExecuteCommands( this->id, commandBufferCount, pCommandBuffers );
}

// State Commands

void CommandBuffer::bindPipeline( VkPipelineBindPoint pipelineBindPoint,
                                  GraphicsPipeline&   pipeline )
{
    assert( this->began );

    vkCmdBindPipeline( this->id, pipelineBindPoint, pipeline.pipeline );
}

void CommandBuffer::bindDescriptorSets( VkPipelineBindPoint    pipelineBindPoint,
                                        GraphicsPipeline&      pipeline,
                                        ResourceLayoutContainer& layout,
                                        uint32_t               firstSet,
                                        uint32_t               descriptorSetCount,
                                        const VkDescriptorSet* pDescriptorSets,
                                        uint32_t               dynamicOffsetCount,
                                        const uint32_t*        pDynamicOffsets )
{
    assert( this->began );

    vkCmdBindDescriptorSets( this->id, pipelineBindPoint, layout.pipelineLayout,
                             firstSet, descriptorSetCount, pDescriptorSets,
                             dynamicOffsetCount, pDynamicOffsets );
}

void CommandBuffer::bindVertexBuffers( uint32_t             firstBinding,
                                       std::vector<Buffer>& buffers,
                                       const VkDeviceSize*  pOffsets )
{
    assert( this->began );

    std::vector<VkBuffer> vkbuffers;
    vkbuffers.resize( buffers.size() );

    for ( auto i = 0; i < vkbuffers.size(); i++ )
    {
        //vkbuffers.emplace_back( buf.id );
        vkbuffers[ i ] = buffers[ i ].id;
    }
    

    vkCmdBindVertexBuffers( this->id, firstBinding, vkbuffers.size(),
                            vkbuffers.data(), pOffsets );
}

void CommandBuffer::bindVertexBuffer( uint32_t     binding,
                                      Buffer&      buffer,
                                      VkDeviceSize offset )
{
    assert( this->began );

    vkCmdBindVertexBuffers( this->id, binding, 1, &buffer.id, &offset );
}

void CommandBuffer::bindIndexBuffer( Buffer&      buffer,
                                     VkDeviceSize offset,
                                     VkIndexType  indexType )
{
    assert( this->began );

    vkCmdBindIndexBuffer( this->id, buffer.id, offset, indexType );
}

// Clear Commands

void CommandBuffer::clearColorImage( VkImage                        image,
                                     VkImageLayout                  imageLayout,
                                     const VkClearColorValue*       pColor,
                                     uint32_t                       rangeCount,
                                     const VkImageSubresourceRange* pRanges )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdClearColorImage( this->id, image, imageLayout, pColor,
                          rangeCount, pRanges );
}

void CommandBuffer::clearDepthStencilImage( VkImage                         image,
                                            VkImageLayout                   imageLayout,
                                            const VkClearDepthStencilValue* pDepthStencil,
                                            uint32_t                        rangeCount,
                                            const VkImageSubresourceRange*  pRanges )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdClearDepthStencilImage( this->id, image, imageLayout,
                                 pDepthStencil, rangeCount, pRanges );
}

void CommandBuffer::clearAttachments( uint32_t                 attachmentCount,
                                      const VkClearAttachment* pAttachments,
                                      uint32_t                 rectCount,
                                      const VkClearRect*       pRects )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdClearAttachments( this->id, attachmentCount, pAttachments,
                           rectCount, pRects );
}

void CommandBuffer::fillBuffer( VkBuffer     dstBuffer,
                                VkDeviceSize dstOffset,
                                VkDeviceSize size,
                                uint32_t     data )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdFillBuffer( this->id, dstBuffer, dstOffset, size, data );
}

void CommandBuffer::updateBuffer( VkBuffer        dstBuffer,
                                  VkDeviceSize    dstOffset,
                                  VkDeviceSize    dataSize,
                                  const uint32_t* pData )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdUpdateBuffer( this->id, dstBuffer, dstOffset, dataSize, pData );
}

// Copy Commands

void CommandBuffer::copyBuffer( VkBuffer            srcBuffer,
                                VkBuffer            dstBuffer,
                                uint32_t            regionCount,
                                const VkBufferCopy* pRegions )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdCopyBuffer( this->id, srcBuffer, dstBuffer, regionCount, pRegions );
}

void CommandBuffer::copyImage( VkImage            srcImage,
                               VkImageLayout      srcImageLayout,
                               VkImage            dstImage,
                               VkImageLayout      dstImageLayout,
                               uint32_t           regionCount,
                               const VkImageCopy* pRegions )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdCopyImage( this->id, srcImage, srcImageLayout, dstImage,
                    dstImageLayout, regionCount, pRegions );
}

void CommandBuffer::copyBufferToImage( VkBuffer                 srcBuffer,
                                       VkImage                  dstImage,
                                       VkImageLayout            dstImageLayout,
                                       uint32_t                 regionCount,
                                       const VkBufferImageCopy* pRegions )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdCopyBufferToImage( this->id, srcBuffer, dstImage, dstImageLayout,
                            regionCount, pRegions );
}

void CommandBuffer::copyImageToBuffer( VkImage                  srcImage,
                                       VkImageLayout            srcImageLayout,
                                       VkBuffer                 dstBuffer,
                                       uint32_t                 regionCount,
                                       const VkBufferImageCopy* pRegions )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdCopyImageToBuffer( this->id, srcImage, srcImageLayout, dstBuffer,
                            regionCount, pRegions );
}

void CommandBuffer::blitImage( VkImage            srcImage,
                               VkImageLayout      srcImageLayout,
                               VkImage            dstImage,
                               VkImageLayout      dstImageLayout,
                               uint32_t           regionCount,
                               const VkImageBlit* pRegions,
                               VkFilter           filter )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdBlitImage( this->id, srcImage, srcImageLayout, dstImage,
                    dstImageLayout, regionCount, pRegions, filter );
}

void CommandBuffer::resolveImage( VkImage               srcImage,
                                  VkImageLayout         srcImageLayout,
                                  VkImage               dstImage,
                                  VkImageLayout         dstImageLayout,
                                  uint32_t              regionCount,
                                  const VkImageResolve* pRegions )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdResolveImage( this->id, srcImage, srcImageLayout, dstImage,
                       dstImageLayout, regionCount, pRegions );
}

// Drawing Commands

void CommandBuffer::draw( uint32_t vertexCount,
                          uint32_t instanceCount,
                          uint32_t firstVertex,
                          uint32_t firstInstance )
{
    assert( this->began && !this->ended && this->renderPass );

    vkCmdDraw( this->id, vertexCount, instanceCount,
               firstVertex, firstInstance );
}

void CommandBuffer::drawIndexed( uint32_t indexCount,
                                 uint32_t instanceCount,
                                 uint32_t firstIndex,
                                 int32_t  vertexOffset,
                                 uint32_t firstInstance )
{
    assert( this->began && !this->ended && this->renderPass );

    vkCmdDrawIndexed( this->id, indexCount, instanceCount,
                      firstIndex, vertexOffset, firstInstance );
}

void CommandBuffer::drawIndirect( VkBuffer     buffer,
                                  VkDeviceSize offset,
                                  uint32_t     drawCount,
                                  uint32_t     stride )
{
    assert( this->began && !this->ended && this->renderPass );

    vkCmdDrawIndirect( this->id, buffer, offset, drawCount, stride );
}
void CommandBuffer::drawIndexedIndirect( VkBuffer     buffer,
                                         VkDeviceSize offset,
                                         uint32_t     drawCount,
                                         uint32_t     stride )
{
    assert( this->began && !this->ended && this->renderPass );

    vkCmdDrawIndexedIndirect( this->id, buffer, offset, drawCount, stride );
}

// Fragment Operations

void CommandBuffer::setScissor( uint32_t        firstScissor,
                                uint32_t        scissorCount,
                                const VkRect2D* pScissors )
{
    assert( this->began );

    vkCmdSetScissor( this->id, firstScissor, scissorCount, pScissors );
}

void CommandBuffer::setDepthBounds( float minDepthBounds,
                                    float maxDepthBounds )
{
    assert( this->began );

    vkCmdSetDepthBounds( this->id, minDepthBounds, maxDepthBounds );
}

void CommandBuffer::setStencilCompareMask( VkStencilFaceFlags faceMask,
                                           uint32_t           compareMask )
{
    assert( this->began );

    vkCmdSetStencilCompareMask( this->id, faceMask, compareMask );
}

void CommandBuffer::setStencilWriteMask( VkStencilFaceFlags faceMask,
                                         uint32_t           writeMask )
{
    assert( this->began );

    vkCmdSetStencilWriteMask( this->id, faceMask, writeMask );
}

void CommandBuffer::setStencilReference( VkStencilFaceFlags faceMask,
                                         uint32_t           reference )
{
    assert( this->began );

    vkCmdSetStencilReference( this->id, faceMask, reference );
}

// Viewport Commands

void CommandBuffer::setViewport( uint32_t          firstViewport,
                                 uint32_t          viewportCount,
                                 const VkViewport* pViewports )
{
    assert( this->began );

    vkCmdSetViewport( this->id, firstViewport, viewportCount, pViewports );
}

// Rasterization Commands

void CommandBuffer::setLineWidth( float lineWidth )
{
    assert( this->began );

    vkCmdSetLineWidth( this->id, lineWidth );
}

void CommandBuffer::setDepthBias( float depthBiasConstantFactor,
                                  float depthBiasClamp,
                                  float depthBiasSlopeFactor )
{
    assert( this->began );

    vkCmdSetDepthBias( this->id, depthBiasConstantFactor,
                       depthBiasClamp, depthBiasSlopeFactor );
}

// Framebuffer Commands

void CommandBuffer::setBlendConstants( const float blendConstants[ 4 ] )
{
    assert( this->began );

    vkCmdSetBlendConstants( this->id, blendConstants );
}

// Dispatch Commands

void CommandBuffer::dispatch( uint32_t x, uint32_t y, uint32_t z )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdDispatch( this->id, x, y, z );
}

void CommandBuffer::dispatchIndirect( VkBuffer buffer, VkDeviceSize offset )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdDispatchIndirect( this->id, buffer, offset );
}

// Synchronization Commands

void CommandBuffer::pipelineBarrier(
    VkPipelineStageFlags         srcStageMask,
    VkPipelineStageFlags         dstStageMask,
    VkDependencyFlags            dependencyFlags,
    uint32_t                     memoryBarrierCount,
    const VkMemoryBarrier*       pMemoryBarriers,
    uint32_t                     bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t                     imageMemoryBarrierCount,
    const VkImageMemoryBarrier*  pImageMemoryBarriers
    )
{
    assert( this->began && !this->ended && !this->renderPass );

    vkCmdPipelineBarrier( this->id,
                          srcStageMask,
                          dstStageMask,
                          dependencyFlags,
                          memoryBarrierCount,
                          pMemoryBarriers,
                          bufferMemoryBarrierCount,
                          pBufferMemoryBarriers,
                          imageMemoryBarrierCount,
                          pImageMemoryBarriers );
}

// PushConstant Commands

void CommandBuffer::pushConstants( VkPipelineLayout   layout,
                                   VkShaderStageFlags stageFlags,
                                   uint32_t           offset,
                                   uint32_t           size,
                                   const void*        pValues )
{
    assert( this->began );

    vkCmdPushConstants( this->id, layout, stageFlags, offset,
                        size, pValues );
}

//TODO: Remove when other cmdbuf methods have been added
VkCommandBuffer* CommandBuffer::getHandle()
{
    return &this->id;
}
