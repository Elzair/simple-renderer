#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "descriptor.hpp"
#include "device.hpp"
#include "buffer.hpp"
#include "image.hpp"
#include "pipeline.hpp"

class CommandPool;
class CommandBuffer;

class Buffer;
class Image;

struct DescriptorSet;

enum class CommandBufferUsage
{
    ONE_TIME,
    SIMULTANEOUS_USE
};

class CommandPool
{
    friend class Device;
    friend class CommandBuffer;
    
public:

    CommandPool() {}

    CommandPool( Device*  device,
                 VkQueue  queue,
                 uint32_t queueIdx )
    {
        this->init( device, queue, queueIdx );
    }

    ~CommandPool(  ) { this->deinit(); }

    void init( Device*  device,
               VkQueue  queue,
               uint32_t queueIdx );

    void deinit();

    void reset();

private:

    VkCommandPool id     = VK_NULL_HANDLE;
    Device*       device = nullptr;
    VkQueue       queue  = VK_NULL_HANDLE;

    void allocateCommandBuffer( VkCommandBuffer* cmdbuf );
    void freeCommandBuffer( VkCommandBuffer* commandBuffer );
};

class CommandBuffer
{
    friend class Buffer;
    friend class CommandPool;
    friend class Image;
    
public:

    CommandBuffer() {}

    CommandBuffer( Device*         device_,
                   VkQueue         queue_,
                   CommandPool*    pool_,
                   VkCommandBuffer buffer_ )
        : device( device_ ),
          queue( queue_ ),
          pool( pool_ ),
          id( buffer_ )
    {}

    CommandBuffer( const CommandBuffer& c )
        : id ( c.id ),
          device( c.device ),
          queue( c.queue ),
          pool( c.pool ),
          began( c.began ),
          renderPass( c.renderPass ),
          ended( c.began )
    {}

    CommandBuffer( CommandBuffer&& c ) noexcept
        : id( c.id ),
          device( c.device ),
          queue( c.queue ),
          pool( c.pool ),
          began( c.began ),
          renderPass( c.renderPass ),
          ended( c.began )
    {}

    CommandBuffer( Device*      device,
                   VkQueue      queue,
                   CommandPool* pool )
    {
        this->init( device, queue, pool );
    }

    ~CommandBuffer() { this->deinit(); }

    void init( Device*      device,
               VkQueue      queue,
               CommandPool* pool );

    void deinit();

    CommandBuffer& operator=( CommandBuffer&& c ) { return *this; }

    // Basic Commands
    void begin(
        CommandBufferUsage usage = CommandBufferUsage::SIMULTANEOUS_USE
        );
    void end();

    // RenderPass Commands
    void beginRenderPass( RenderPass&                renderPass,
                          VkFramebuffer              framebuffer,
                          VkRect2D                   renderArea,
                          std::vector<VkClearValue>& clearValues,
                          VkSubpassContents          contents );
    void nextSubpass( VkSubpassContents contents );
    void endRenderPass();

    // Execution Commands
    void executeCommands( uint32_t               commandBufferCount,
                          const VkCommandBuffer* pCommandBuffers );

    // State Commands
    void bindPipeline( VkPipelineBindPoint pipelineBindPoint,
                       GraphicsPipeline&   pipeline );
    void bindDescriptorSets( VkPipelineBindPoint               pipelineBindPoint,
                             GraphicsPipeline&                 pipeline,
                             PipelineLayout&                   layout,
                             uint32_t                          firstSet,
                             //uint32_t               descriptorSetCount,
                             //const VkDescriptorSet* pDescriptorSets,
                             const std::vector<DescriptorSet>& descriptorSets,
                             uint32_t                          dynamicOffsetCount,
                             const uint32_t*                   pDynamicOffsets );
    void bindVertexBuffers( uint32_t             firstBinding,
                            std::vector<Buffer>& buffers,
                            const VkDeviceSize*  pOffsets );
    void bindVertexBuffer( uint32_t     binding,
                           Buffer&      buffer,
                           VkDeviceSize offset );
    void bindIndexBuffer( Buffer&      buffer,
                          VkDeviceSize offset,
                          VkIndexType  indexType );

    // Clear Commands
    void clearColorImage( VkImage                        image,
                          VkImageLayout                  imageLayout,
                          const VkClearColorValue*       pColor,
                          uint32_t                       rangeCount,
                          const VkImageSubresourceRange* pRanges );
    void clearDepthStencilImage( VkImage                         image,
                                 VkImageLayout                   imageLayout,
                                 const VkClearDepthStencilValue* pDepthStencil,
                                 uint32_t                        rangeCount,
                                 const VkImageSubresourceRange*  pRanges );
    void clearAttachments( uint32_t                 attachmentCount,
                           const VkClearAttachment* pAttachments,
                           uint32_t                 rectCount,
                           const VkClearRect*       pRects );
    void fillBuffer( VkBuffer     dstBuffer,
                     VkDeviceSize dstOffset,
                     VkDeviceSize size,
                     uint32_t     data );
    void updateBuffer( VkBuffer        dstBuffer,
                       VkDeviceSize    dstOffset,
                       VkDeviceSize    dataSize,
                       const uint32_t* pData );

    // Copy Commands
    void copyBuffer( VkBuffer            srcBuffer,
                     VkBuffer            dstBuffer,
                     uint32_t            regionCount,
                     const VkBufferCopy* pRegions );
    void copyImage( VkImage            srcImage,
                    VkImageLayout      srcImageLayout,
                    VkImage            dstImage,
                    VkImageLayout      dstImageLayout,
                    uint32_t           regionCount,
                    const VkImageCopy* pRegions );
    void copyBufferToImage( VkBuffer                 srcBuffer,
                            VkImage                  dstImage,
                            VkImageLayout            dstImageLayout,
                            uint32_t                 regionCount,
                            const VkBufferImageCopy* pRegions );
    void copyImageToBuffer( VkImage                  srcImage,
                            VkImageLayout            srcImageLayout,
                            VkBuffer                 dstBuffer,
                            uint32_t                 regionCount,
                            const VkBufferImageCopy* pRegions );
    void blitImage( VkImage            srcImage,
                    VkImageLayout      srcImageLayout,
                    VkImage            dstImage,
                    VkImageLayout      dstImageLayout,
                    uint32_t           regionCount,
                    const VkImageBlit* pRegions,
                    VkFilter           filter );
    void resolveImage( VkImage               srcImage,
                       VkImageLayout         srcImageLayout,
                       VkImage               dstImage,
                       VkImageLayout         dstImageLayout,
                       uint32_t              regionCount,
                       const VkImageResolve* pRegions );

    // Drawing Commands
    void draw( uint32_t vertexCount,
               uint32_t instanceCount,
               uint32_t firstVertex,
               uint32_t firstInstance );
    void drawIndexed( uint32_t indexCount,
                      uint32_t instanceCount,
                      uint32_t firstIndex,
                      int32_t  vertexOffset,
                      uint32_t firstInstance );
    void drawIndirect( VkBuffer     buffer,
                       VkDeviceSize offset,
                       uint32_t     drawCount,
                       uint32_t     stride );
    void drawIndexedIndirect( VkBuffer     buffer,
                              VkDeviceSize offset,
                              uint32_t     drawCount,
                              uint32_t     stride );

    // Fragment Operations
    void setScissor( uint32_t        firstScissor,
                     uint32_t        scissorCount,
                     const VkRect2D* pScissors );
    void setDepthBounds( float minDepthBounds,
                         float maxDepthBounds );
    void setStencilCompareMask( VkStencilFaceFlags faceMask,
                                uint32_t           compareMask );
    void setStencilWriteMask( VkStencilFaceFlags faceMask,
                              uint32_t           writeMask );
    void setStencilReference( VkStencilFaceFlags faceMask,
                              uint32_t           reference );

    // Viewport Commands
    void setViewport( uint32_t          firstViewport,
                      uint32_t          viewportCount,
                      const VkViewport* pViewports );

    // Rasterization Commands
    void setLineWidth( float lineWidth );
    void setDepthBias( float depthBiasConstantFactor,
                       float depthBiasClamp,
                       float depthBiasSlopeFactor );

    // Framebuffer Commands
    void setBlendConstants( const float blendConstants[ 4 ] );

    // Dispatch Commands
    void dispatch( uint32_t x, uint32_t y, uint32_t z );
    void dispatchIndirect( VkBuffer buffer, VkDeviceSize offset );

    // Synchronization Commands
    void pipelineBarrier( VkPipelineStageFlags         srcStageMask,
                          VkPipelineStageFlags         dstStageMask,
                          VkDependencyFlags            dependencyFlags,
                          uint32_t                     memoryBarrierCount,
                          const VkMemoryBarrier*       pMemoryBarriers,
                          uint32_t                     bufferMemoryBarrierCount,
                          const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                          uint32_t                     imageMemoryBarrierCount,
                          const VkImageMemoryBarrier*  pImageMemoryBarriers );

    // PushConstant Commands
    void pushConstants( VkPipelineLayout   layout,
                        VkShaderStageFlags stageFlags,
                        uint32_t           offset,
                        uint32_t           size,
                        const void*        pValues );

    //TODO: Remove when other cmdbuf methods have been added
    VkCommandBuffer* getHandle();

private:

    VkCommandBuffer id         = VK_NULL_HANDLE;
    Device*         device     = nullptr;
    VkQueue         queue      = VK_NULL_HANDLE;
    CommandPool*    pool       = nullptr;
    bool            began      = false;
    bool            renderPass = false;
    bool            ended      = false;
};
