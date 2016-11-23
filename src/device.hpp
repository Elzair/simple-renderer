#pragma once

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
            const std::vector<const char*> validationLayers )
    {
        this->init( physical, surface, extensions, validationLayers );
    }

    Device() {}

    ~Device() { this->deinit(); }

    void init( VkPhysicalDevice               physical,
               VkSurfaceKHR                   surface,
               const std::vector<const char*> extensions,
               const std::vector<const char*> validationLayers );

    void deinit();

    /*
     * Wrappers around vkFn(VkDevice,..) functions
     */

    // Device
    VkResult waitIdle();

    // Queues
    void getDeviceQueue( uint32_t queueFamilyIndex,
                         uint32_t queueIndex,
                         VkQueue* pQueue );
    VkResult queueWaitIdle( VkQueue queue );

    // Command Buffers
    VkResult createCommandPool( const VkCommandPoolCreateInfo* pCreateInfo,
                                VkCommandPool*                 pCommandPool );
    VkResult resetCommandPool( VkCommandPool           commandPool,
                               VkCommandPoolResetFlags flags );
    void destroyCommandPool( VkCommandPool commandPool );
    VkResult allocateCommandBuffers( VkCommandBuffer* pCommandBuffers );
    void freeCommandBuffers( VkCommandPool          commandPool,
                             uint32_t               commandBufferCount,
                             const VkCommandBuffer* pCommandBuffers );

    // Semaphores
    VkResult createSemaphore( const VkSemaphoreCreateInfo* pCreateInfo,
                              VkSemaphore*                 pSemaphore );
    void destroySemaphore( VkSemaphore semaphore );

    // Render Pass
    VkResult createRenderPass( const VkRenderPassCreateInfo* pCreateInfo,
                               VkRenderPass*                 pRenderPass );
    void destroyRenderPass( VkRenderPass renderPass );

    // Framebuffer
    VkResult createFramebuffer( const VkFramebufferCreateInfo* pCreateInfo,
                                VkFramebuffer*                 pFramebuffer);
    void destroyFramebuffer( VkFramebuffer framebuffer );

    // Shader
    VkResult createShaderModule( const VkShaderModuleCreateInfo* pCreateInfo,
                                 VkShaderModule*                 pShaderModule );
    void destroyShaderModule( VkShaderModule shaderModule );

    // Pipeline
    VkResult createComputePipelines( VkPipelineCache                    pipelineCache,
                                     uint32_t                           createInfoCount,
                                     const VkComputePipelineCreateInfo* pCreateInfos,
                                     VkPipeline*                        pPipelines );
    VkResult createGraphicsPipelines( VkPipelineCache                     pipelineCache,
                                      uint32_t                            createInfoCount,
                                      const VkGraphicsPipelineCreateInfo* pCreateInfos,
                                      VkPipeline*                         pPipelines );
    void destroyPipeline( VkPipeline pipeline );
    VkResult createPipelineCache( const VkPipelineCacheCreateInfo* pCreateInfo,
                                  VkPipelineCache*                 pPipelineCache );
    VkResult mergePipelineCaches( VkPipelineCache        dstCache,
                                  uint32_t               srcCacheCount,
                                  const VkPipelineCache* pSrcCaches );
    VkResult getPipelineCacheData( VkPipelineCache pipelineCache,
                                   std::size_t*    pDataSize,
                                   void*           pData );
    void destroyPipelineCache( VkPipelineCache pipelineCache );

    // Memory
    VkResult allocateMemory( const VkMemoryAllocateInfo* pAllocateInfo,
                             VkDeviceMemory*             pMemory );
    void freeMemory( VkDeviceMemory memory );
    VkResult mapMemory( VkDeviceMemory   memory,
                        VkDeviceSize     offset,
                        VkDeviceSize     size,
                        VkMemoryMapFlags flags,
                        void**           ppData ); 
    VkResult flushMappedMemoryRanges( uint32_t                   memoryRangeCount,
                                      const VkMappedMemoryRange* pMemoryRanges );
    VkResult invalidateMappedMemoryRanges( uint32_t                   memoryRangeCount,
                                           const VkMappedMemoryRange* pMemoryRanges );
    void unmapMemory( VkDeviceMemory memory );
    void getDeviceMemoryCommitment( VkDeviceMemory memory,
                                    VkDeviceSize*  pCommittedMemoryInBytes );

    // Resource
    VkResult createBuffer( const VkBufferCreateInfo* pCreateInfo,
                           VkBuffer*                 pBuffer );
    void destroyBuffer( VkBuffer buffer );
    VkResult createBufferViewe( const VkBufferViewCreateInfo* pCreateInfo,
                                VkBufferView*                 pView );
    void destroyBufferView( VkBufferView bufferView );
    VkResult createImage( const VkImageCreateInfo* pCreateInfo,
                          VkImage*                 pImage );
    void getImageSubresourceLayout( VkImage                   image,
                                    const VkImageSubresource* pSubResource,
                                    VkSubresourceLayout*      pLayout );
    void destroyImage( VkImage image );
    VkResult createImageView( const VkImageViewCreateInfo* pCreateInfo,
                              VkImageView*                 pView );
    void destroyImageView( VkImageView imageView );
    void getBufferMemoryRequirements( VkBuffer              buffer,
                                      VkMemoryRequirements* pMemoryRequirements );
    void getImageMemoryRequirements( VkImage               image,
                                     VkMemoryRequirements* pMemoryRequirements );
    VkResult bindBufferMemory( VkBuffer       buffer,
                               VkDeviceMemory memory,
                               VkDeviceSize   memoryOffset );
    VkResult bindImageMemory( VkImage         image,
                               VkDeviceMemory memory,
                               VkDeviceSize   memoryOffset );

    // Sampler
    VkResult createSampler( const VkSamplerCreateInfo* pCreateInfo,
                            VkSampler*                 pSampler );
    void destroySampler( VkSampler sampler );

    // Descriptor
    VkResult createDescriptorSetLayout( const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                        VkDescriptorSetLayout*                 pSetLayout );
    void destroyDescriptorSetLayout( VkDescriptorSetLayout descriptorSetLayout );
    VkResult createPipelineLayout( const VkPipelineLayoutCreateInfo* pCreateInfo,
                                   VkPipelineLayout*                 pPipelineLayout );
    void destroyPipelineLayout( VkPipelineLayout pipelineLayout );
    VkResult createDescriptorPool( const VkDescriptorPoolCreateInfo* pCreateInfo,
                                   VkDescriptorPool*                 pDescriptorPool );
    void destroyDescriptorPool( VkDescriptorPool descriptorPool );
    VkResult allocateDescriptorSets( const VkDescriptorSetAllocateInfo* pAllocateInfo,
                                     VkDescriptorSet*                   pDescriptorSets );
    VkResult freeDescriptorSets( VkDescriptorPool       descriptorPool,
                                 uint32_t               descriptorSetCount,
                                 const VkDescriptorSet* pDescriptorSets );
    VkResult resetDescriptorPool( VkDescriptorPool           descriptorPool,
                                  VkDescriptorPoolResetFlags flags );
    void updateDescriptorSets( uint32_t                    descriptorWriteCount,
                               const VkWriteDescriptorSet* pDescriptorWrites,
                               uint32_t                    descriptorCopyCount,
                               const VkCopyDescriptorSet*  pDescriptorCopies );
};
