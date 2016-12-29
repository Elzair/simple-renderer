#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class Device
{
    friend class Buffer;
    friend class CommandPool;
    friend class Image;
    friend class GraphicsPipeline;
    friend class GraphicsShader;
    friend class RenderPass;
    friend class ResourceLayoutContainer;
    friend class ResourcePool;
    friend class SwapChain;
    
public:

    int              graphicsQueueIdx = -1;
    int              presentQueueIdx  = -1;

    VkQueue          graphicsQueue = VK_NULL_HANDLE;
    VkQueue          presentQueue  = VK_NULL_HANDLE;

    Device( VkPhysicalDevice               physicalDevice,
            VkSurfaceKHR                   surface,
            const std::vector<const char*> extensions,
            const std::vector<const char*> validationLayers )
    {
        this->init( physicalDevice, surface, extensions, validationLayers );
    }

    Device() {}

    ~Device() { this->deinit(); }

    void init( VkPhysicalDevice               physicalDevice,
               VkSurfaceKHR                   surface,
               const std::vector<const char*> extensions,
               const std::vector<const char*> validationLayers );

    void deinit();

    /*
     * Wrappers around vkFn(VkDevice,..) functions
     */

    // Device Methods
    VkResult waitIdle();

    // Queue Methods
    void getDeviceQueue( uint32_t queueFamilyIndex,
                         uint32_t queueIndex,
                         VkQueue* pQueue );
    VkResult queueWaitIdle( VkQueue queue );
    VkResult queueSubmit( VkQueue             queue,
                          uint32_t            submitCount,
                          const VkSubmitInfo* pSubmits,
                          VkFence             fence );
    VkResult queuePresent( VkQueue                 queue,
                           const VkPresentInfoKHR* pPresentInfo );

    // Semaphores Methods
    VkResult createSemaphore( const VkSemaphoreCreateInfo* pCreateInfo,
                              VkSemaphore*                 pSemaphore );
    void destroySemaphore( VkSemaphore semaphore );

    // Descriptor Methods
    VkResult createDescriptorSetLayout(
        const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
        VkDescriptorSetLayout*                 pSetLayout
        );
    void destroyDescriptorSetLayout( VkDescriptorSetLayout descriptorSetLayout );
    VkResult createPipelineLayout(
        const VkPipelineLayoutCreateInfo* pCreateInfo,
        VkPipelineLayout*                 pPipelineLayout
        );
    void destroyPipelineLayout( VkPipelineLayout pipelineLayout );
    VkResult createDescriptorPool(
        const VkDescriptorPoolCreateInfo* pCreateInfo,
        VkDescriptorPool*                 pDescriptorPool
        );
    void destroyDescriptorPool( VkDescriptorPool descriptorPool );
    VkResult allocateDescriptorSets(
        const VkDescriptorSetAllocateInfo* pAllocateInfo,
        VkDescriptorSet*                   pDescriptorSets
        );
    VkResult freeDescriptorSets( VkDescriptorPool       descriptorPool,
                                 uint32_t               descriptorSetCount,
                                 const VkDescriptorSet* pDescriptorSets );
    VkResult resetDescriptorPool( VkDescriptorPool           descriptorPool,
                                  VkDescriptorPoolResetFlags flags );
    void updateDescriptorSets( uint32_t                    descriptorWriteCount,
                               const VkWriteDescriptorSet* pDescriptorWrites,
                               uint32_t                    descriptorCopyCount,
                               const VkCopyDescriptorSet*  pDescriptorCopies );

    // Swapchain Methods
    VkResult createSwapchain( const VkSwapchainCreateInfoKHR* pCreateInfo,
                              VkSwapchainKHR*                 pSwapchain );
    void destroySwapchain( VkSwapchainKHR swapchain );
    VkResult createSharedSwapchains(
        uint32_t                        swapchainCount,
        const VkSwapchainCreateInfoKHR* pCreateInfos,
        VkSwapchainKHR*                 pSwapchains
        );
    VkResult getSwapchainImages( VkSwapchainKHR swapchain,
                                 uint32_t*      pSwapchainImageCount,
                                 VkImage*       pSwapchainImages );
    VkResult acquireNextImage( VkSwapchainKHR swapchain,
                               uint64_t       timeout,
                               VkSemaphore    semaphore,
                               VkFence        fence,
                               uint32_t*      pImageIndex );

    // Sampler Methods
    VkResult createSampler( const VkSamplerCreateInfo* pCreateInfo,
                            VkSampler*                 pSampler );
    void destroySampler( VkSampler sampler );

private:

    VkDevice id                     = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    // Render Pass Methods
    VkResult createRenderPass( const VkRenderPassCreateInfo* pCreateInfo,
                               VkRenderPass*                 pRenderPass );
    void destroyRenderPass( VkRenderPass renderPass );

    // Framebuffer Methods
    VkResult createFramebuffer( const VkFramebufferCreateInfo* pCreateInfo,
                                VkFramebuffer*                 pFramebuffer);
    void destroyFramebuffer( VkFramebuffer framebuffer );

    // Shader Methods
    VkResult createShaderModule( const VkShaderModuleCreateInfo* pCreateInfo,
                                 VkShaderModule*                 pShaderModule );
    void destroyShaderModule( VkShaderModule shaderModule );

    // Pipeline Methods
    VkResult createComputePipelines(
        VkPipelineCache                    pipelineCache,
        uint32_t                           createInfoCount,
        const VkComputePipelineCreateInfo* pCreateInfos,
        VkPipeline*                        pPipelines
        );
    VkResult createGraphicsPipelines(
        VkPipelineCache                     pipelineCache,
        uint32_t                            createInfoCount,
        const VkGraphicsPipelineCreateInfo* pCreateInfos,
        VkPipeline*                         pPipelines
        );
    void destroyPipeline( VkPipeline pipeline );
    VkResult createPipelineCache(
        const VkPipelineCacheCreateInfo* pCreateInfo,
        VkPipelineCache*                 pPipelineCache
        );
    VkResult mergePipelineCaches( VkPipelineCache        dstCache,
                                  uint32_t               srcCacheCount,
                                  const VkPipelineCache* pSrcCaches );
    VkResult getPipelineCacheData( VkPipelineCache pipelineCache,
                                   std::size_t*    pDataSize,
                                   void*           pData );
    void destroyPipelineCache( VkPipelineCache pipelineCache );

    // Memory Methods
    VkResult allocateMemory( const VkMemoryAllocateInfo* pAllocateInfo,
                             VkDeviceMemory*             pMemory );
    void freeMemory( VkDeviceMemory memory );
    VkResult mapMemory( VkDeviceMemory   memory,
                        VkDeviceSize     offset,
                        VkDeviceSize     size,
                        VkMemoryMapFlags flags,
                        void**           ppData ); 
    VkResult flushMappedMemoryRanges(
        uint32_t                   memoryRangeCount,
        const VkMappedMemoryRange* pMemoryRanges
        );
    VkResult invalidateMappedMemoryRanges(
        uint32_t                   memoryRangeCount,
        const VkMappedMemoryRange* pMemoryRanges
        );
    void unmapMemory( VkDeviceMemory memory );
    void getDeviceMemoryCommitment( VkDeviceMemory memory,
                                    VkDeviceSize*  pCommittedMemoryInBytes );

    // Buffer Methods
    VkResult createBuffer( const VkBufferCreateInfo* pCreateInfo,
                           VkBuffer*                 pBuffer );
    void destroyBuffer( VkBuffer buffer );
    VkResult createBufferViewe( const VkBufferViewCreateInfo* pCreateInfo,
                                VkBufferView*                 pView );
    void destroyBufferView( VkBufferView bufferView );
    void getBufferMemoryRequirements(
        VkBuffer              buffer,
        VkMemoryRequirements* pMemoryRequirements
        );
    VkResult bindBufferMemory( VkBuffer       buffer,
                               VkDeviceMemory memory,
                               VkDeviceSize   memoryOffset );

    // Image Methods
    VkResult createImage( const VkImageCreateInfo* pCreateInfo,
                          VkImage*                 pImage );
    void getImageSubresourceLayout( VkImage                   image,
                                    const VkImageSubresource* pSubResource,
                                    VkSubresourceLayout*      pLayout );
    void destroyImage( VkImage image );
    VkResult createImageView( const VkImageViewCreateInfo* pCreateInfo,
                              VkImageView*                 pView );
    void destroyImageView( VkImageView imageView );
    void getImageMemoryRequirements( VkImage               image,
                                     VkMemoryRequirements* pMemoryRequirements );
    VkResult bindImageMemory( VkImage         image,
                              VkDeviceMemory memory,
                              VkDeviceSize   memoryOffset );

    // Command Buffer Methods
    VkResult createCommandPool( const VkCommandPoolCreateInfo* pCreateInfo,
                                VkCommandPool*                 pCommandPool );
    VkResult resetCommandPool( VkCommandPool           commandPool,
                               VkCommandPoolResetFlags flags );
    void destroyCommandPool( VkCommandPool commandPool );
    VkResult allocateCommandBuffers(
        const VkCommandBufferAllocateInfo* pAllocateInfo,
        VkCommandBuffer*                   pCommandBuffers
        );
    void freeCommandBuffers( VkCommandPool          commandPool,
                             uint32_t               commandBufferCount,
                             const VkCommandBuffer* pCommandBuffers );

};
