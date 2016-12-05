#include <set>
#include "common.hpp"
#include "utils.hpp"
#include "device.hpp"

void Device::init( VkPhysicalDevice               physical,
                   VkSurfaceKHR                   surface,
                   const std::vector<const char*> extensions,
                   const std::vector<const char*> validationLayers )
{
    // Create queues for both the graphics and presentation families
    QueueFamilyIndices indices = FindQueueFamilies( physical,
                                                    surface );
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = {
        indices.graphicsFamily,
        indices.presentFamily
    };
    this->graphicsQueueIdx = indices.graphicsFamily;
    this->presentQueueIdx  = indices.presentFamily;
        
    float queuePriority = 1.0f;

    for ( int queueFamily : uniqueQueueFamilies )
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount       = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back( queueCreateInfo );
    }

    VkPhysicalDeviceFeatures devFeatures = {};

    // Create struct used to create a logical device
    VkDeviceCreateInfo devCreateInfo = {};
    devCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devCreateInfo.pQueueCreateInfos       = queueCreateInfos.data();
    devCreateInfo.queueCreateInfoCount    = (uint32_t)queueCreateInfos.size();
    devCreateInfo.pEnabledFeatures        = &devFeatures;
    devCreateInfo.enabledExtensionCount   = extensions.size();
    devCreateInfo.ppEnabledExtensionNames = extensions.data();
    if ( validationLayers.size() > 0 )
    {
        devCreateInfo.enabledLayerCount   = validationLayers.size();
        devCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        devCreateInfo.enabledLayerCount = 0;
    }

    VK_CHECK_RESULT( vkCreateDevice( physical,
                                     &devCreateInfo,
                                     nullptr,
                                     &this->id ) );

    // Retrieve handles for graphics and presentation queues
    vkGetDeviceQueue( this->id, this->graphicsQueueIdx,
                      0, &this->graphicsQueue);
    vkGetDeviceQueue( this->id, this->presentQueueIdx,
                      0, &this->presentQueue );
}

void Device::deinit()
{
    if ( this->id != VK_NULL_HANDLE )
    {
        vkDestroyDevice( this->id, nullptr );
    }
}

/*
 * Wrappers around vkFn(VkDevice,..) functions
 */

// Device Methods

VkResult Device::waitIdle()
{
    return vkDeviceWaitIdle( this->id );
}

// Queue Methods

void Device::getDeviceQueue( uint32_t queueFamilyIndex,
                             uint32_t queueIndex,
                             VkQueue* pQueue )
{
    vkGetDeviceQueue( this->id, queueFamilyIndex, queueIndex, pQueue );
}

VkResult Device::queueWaitIdle( VkQueue queue )
{
    return vkQueueWaitIdle( queue );
}

VkResult Device::queueSubmit( VkQueue             queue,
                              uint32_t            submitCount,
                              const VkSubmitInfo* pSubmits,
                              VkFence             fence )
{
    return vkQueueSubmit( queue, submitCount, pSubmits, fence );
}

VkResult Device::queuePresent( VkQueue                 queue,
                               const VkPresentInfoKHR* pPresentInfo )
{
    return vkQueuePresentKHR( queue, pPresentInfo );
}

// Command Buffer Methods

VkResult Device::createCommandPool( const VkCommandPoolCreateInfo* pCreateInfo,
                                    VkCommandPool*                 pCommandPool )
{
    return vkCreateCommandPool( this->id, pCreateInfo, nullptr, pCommandPool );
}

VkResult Device::resetCommandPool( VkCommandPool           commandPool,
                                   VkCommandPoolResetFlags flags )
{
    return vkResetCommandPool( this->id, commandPool, flags );
}

void Device::destroyCommandPool( VkCommandPool commandPool )
{
    vkDestroyCommandPool( this->id, commandPool, nullptr );
}

VkResult Device::allocateCommandBuffers( const VkCommandBufferAllocateInfo* pAllocateInfo,
                                         VkCommandBuffer*                   pCommandBuffers )
{
    return vkAllocateCommandBuffers( this->id, pAllocateInfo, pCommandBuffers );
}

void Device::freeCommandBuffers( VkCommandPool          commandPool,
                                 uint32_t               commandBufferCount,
                                 const VkCommandBuffer* pCommandBuffers )
{
    vkFreeCommandBuffers( this->id, commandPool, commandBufferCount, pCommandBuffers );
}

// Semaphore Methods

VkResult Device::createSemaphore( const VkSemaphoreCreateInfo* pCreateInfo,
                                  VkSemaphore*                 pSemaphore )
{
    return vkCreateSemaphore( this->id, pCreateInfo, nullptr, pSemaphore );
}

void Device::destroySemaphore( VkSemaphore semaphore )
{
    vkDestroySemaphore( this->id, semaphore, nullptr );
}

// Render Pass Methods

VkResult Device::createRenderPass( const VkRenderPassCreateInfo* pCreateInfo,
                                   VkRenderPass*                 pRenderPass )
{
    return vkCreateRenderPass( this->id, pCreateInfo, nullptr, pRenderPass );
}

void Device::destroyRenderPass( VkRenderPass renderPass )
{
    vkDestroyRenderPass( this->id, renderPass, nullptr );
}

// Framebuffer Methods
VkResult Device::createFramebuffer( const VkFramebufferCreateInfo* pCreateInfo,
                                    VkFramebuffer*                 pFramebuffer)
{
    return vkCreateFramebuffer( this->id, pCreateInfo, nullptr, pFramebuffer );
}

void Device::destroyFramebuffer( VkFramebuffer framebuffer )
{
    vkDestroyFramebuffer( this->id, framebuffer, nullptr );
}

// Shader Methods
VkResult Device::createShaderModule( const VkShaderModuleCreateInfo* pCreateInfo,
                                     VkShaderModule*                 pShaderModule )
{
    return vkCreateShaderModule( this->id, pCreateInfo, nullptr, pShaderModule );
}

void Device::destroyShaderModule( VkShaderModule shaderModule )
{
    vkDestroyShaderModule( this->id, shaderModule, nullptr );
}

// Pipeline Methods
VkResult Device::createComputePipelines( VkPipelineCache                    pipelineCache,
                                         uint32_t                           createInfoCount,
                                         const VkComputePipelineCreateInfo* pCreateInfos,
                                         VkPipeline*                        pPipelines )
{
    return vkCreateComputePipelines( this->id, pipelineCache, createInfoCount,
                                     pCreateInfos, nullptr, pPipelines );
}

VkResult Device::createGraphicsPipelines( VkPipelineCache                    pipelineCache,
                                          uint32_t                           createInfoCount,
                                          const VkGraphicsPipelineCreateInfo* pCreateInfos,
                                          VkPipeline*                        pPipelines )
{
    return vkCreateGraphicsPipelines( this->id, pipelineCache, createInfoCount,
                                      pCreateInfos, nullptr, pPipelines );
}

void Device::destroyPipeline( VkPipeline pipeline )
{
    vkDestroyPipeline( this->id, pipeline, nullptr );
}

VkResult Device::createPipelineCache( const VkPipelineCacheCreateInfo* pCreateInfo,
                                      VkPipelineCache*                 pPipelineCache )
{
    return vkCreatePipelineCache( this->id, pCreateInfo, nullptr, pPipelineCache );
}

VkResult Device::mergePipelineCaches( VkPipelineCache        dstCache,
                                      uint32_t               srcCacheCount,
                                      const VkPipelineCache* pSrcCaches )
{
    return vkMergePipelineCaches( this->id, dstCache, srcCacheCount, pSrcCaches );
}

VkResult Device::getPipelineCacheData( VkPipelineCache pipelineCache,
                                       std::size_t*    pDataSize,
                                       void*           pData )
{
    return vkGetPipelineCacheData( this->id, pipelineCache, pDataSize, pData );
}

void Device::destroyPipelineCache( VkPipelineCache pipelineCache )
{
    vkDestroyPipelineCache( this->id, pipelineCache, nullptr );
}

// Memory Methods

VkResult Device::allocateMemory( const VkMemoryAllocateInfo* pAllocateInfo,
                                 VkDeviceMemory*             pMemory )
{
    return vkAllocateMemory( this->id, pAllocateInfo, nullptr, pMemory );
}

void Device::freeMemory( VkDeviceMemory memory )
{
    vkFreeMemory( this->id, memory, nullptr );
}

VkResult Device::mapMemory( VkDeviceMemory   memory,
                            VkDeviceSize     offset,
                            VkDeviceSize     size,
                            VkMemoryMapFlags flags,
                            void**           ppData )
{
    return vkMapMemory( this->id, memory, offset, size, flags, ppData );
}

VkResult Device::flushMappedMemoryRanges( uint32_t                   memoryRangeCount,
                                          const VkMappedMemoryRange* pMemoryRanges )
{
    return vkFlushMappedMemoryRanges( this->id, memoryRangeCount, pMemoryRanges );
}

VkResult Device::invalidateMappedMemoryRanges( uint32_t                   memoryRangeCount,
                                               const VkMappedMemoryRange* pMemoryRanges )
{
    return vkInvalidateMappedMemoryRanges( this->id, memoryRangeCount, pMemoryRanges );
}

void Device::unmapMemory( VkDeviceMemory memory )
{
    vkUnmapMemory( this->id, memory );
}

void Device::getDeviceMemoryCommitment( VkDeviceMemory memory,
                                        VkDeviceSize*  pCommittedMemoryInBytes )
{
    vkGetDeviceMemoryCommitment( this->id, memory, pCommittedMemoryInBytes );
}

// Resource Methods

VkResult Device::createBuffer( const VkBufferCreateInfo* pCreateInfo,
                               VkBuffer*                 pBuffer )
{
    return vkCreateBuffer( this->id, pCreateInfo, nullptr, pBuffer );
}

void Device::destroyBuffer( VkBuffer buffer )
{
    vkDestroyBuffer( this->id, buffer, nullptr );
}

VkResult Device::createBufferViewe( const VkBufferViewCreateInfo* pCreateInfo,
                                    VkBufferView*                 pView )
{
    return vkCreateBufferView( this->id, pCreateInfo, nullptr, pView );
}

void Device::destroyBufferView( VkBufferView bufferView )
{
    vkDestroyBufferView( this->id, bufferView, nullptr );
}

VkResult Device::createImage( const VkImageCreateInfo* pCreateInfo,
                              VkImage*                 pImage )
{
    return vkCreateImage( this->id, pCreateInfo, nullptr, pImage );
}

void Device::getImageSubresourceLayout( VkImage                   image,
                                        const VkImageSubresource* pSubResource,
                                        VkSubresourceLayout*      pLayout )
{
    return vkGetImageSubresourceLayout( this->id, image, pSubResource, pLayout );
}

void Device::destroyImage( VkImage image )
{
    vkDestroyImage( this->id, image, nullptr );
}

VkResult Device::createImageView( const VkImageViewCreateInfo* pCreateInfo,
                                  VkImageView*                 pView )
{
    return vkCreateImageView( this->id, pCreateInfo, nullptr, pView );
}

void Device::destroyImageView( VkImageView imageView )
{
    vkDestroyImageView( this->id, imageView, nullptr );
}

void Device::getBufferMemoryRequirements( VkBuffer              buffer,
                                          VkMemoryRequirements* pMemoryRequirements )
{
    vkGetBufferMemoryRequirements( this->id, buffer, pMemoryRequirements );
}

void Device::getImageMemoryRequirements( VkImage               image,
                                         VkMemoryRequirements* pMemoryRequirements )
{
    vkGetImageMemoryRequirements( this->id, image, pMemoryRequirements );
}

VkResult Device::bindBufferMemory( VkBuffer       buffer,
                                   VkDeviceMemory memory,
                                   VkDeviceSize   memoryOffset )
{
    return vkBindBufferMemory( this->id, buffer, memory, memoryOffset );
}

VkResult Device::bindImageMemory( VkImage        image,
                                  VkDeviceMemory memory,
                                  VkDeviceSize   memoryOffset )
{
    return vkBindImageMemory( this->id, image, memory, memoryOffset );
}

// Sampler Methods

VkResult Device::createSampler( const VkSamplerCreateInfo* pCreateInfo,
                                VkSampler*                 pSampler )
{
    return vkCreateSampler( this->id, pCreateInfo, nullptr, pSampler );
}

void Device::destroySampler( VkSampler sampler )
{
    vkDestroySampler( this->id, sampler, nullptr );
}

// Descriptor Methods

VkResult Device::createDescriptorSetLayout( const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                            VkDescriptorSetLayout*                 pSetLayout )
{
    return vkCreateDescriptorSetLayout( this->id, pCreateInfo, nullptr, pSetLayout );
}

void Device::destroyDescriptorSetLayout( VkDescriptorSetLayout descriptorSetLayout )
{
    vkDestroyDescriptorSetLayout( this->id, descriptorSetLayout, nullptr );
}

VkResult Device::createPipelineLayout( const VkPipelineLayoutCreateInfo* pCreateInfo,
                                       VkPipelineLayout*                 pPipelineLayout )
{
    return vkCreatePipelineLayout( this->id, pCreateInfo, nullptr, pPipelineLayout );
}

void Device::destroyPipelineLayout( VkPipelineLayout pipelineLayout )
{
    vkDestroyPipelineLayout( this->id, pipelineLayout, nullptr );
}

VkResult Device::createDescriptorPool( const VkDescriptorPoolCreateInfo* pCreateInfo,
                                       VkDescriptorPool*                 pDescriptorPool )
{
    return vkCreateDescriptorPool( this->id, pCreateInfo, nullptr, pDescriptorPool );
}

void Device::destroyDescriptorPool( VkDescriptorPool descriptorPool )
{
    vkDestroyDescriptorPool( this->id, descriptorPool, nullptr );
}

VkResult Device::allocateDescriptorSets( const VkDescriptorSetAllocateInfo* pAllocateInfo,
                                         VkDescriptorSet*                   pDescriptorSets )
{
    return vkAllocateDescriptorSets( this->id, pAllocateInfo, pDescriptorSets );
}

VkResult Device::freeDescriptorSets( VkDescriptorPool       descriptorPool,
                                     uint32_t               descriptorSetCount,
                                     const VkDescriptorSet* pDescriptorSets )
{
    return vkFreeDescriptorSets( this->id, descriptorPool,
                                 descriptorSetCount, pDescriptorSets );
}

VkResult Device::resetDescriptorPool( VkDescriptorPool           descriptorPool,
                                      VkDescriptorPoolResetFlags flags )
{
    return vkResetDescriptorPool( this->id, descriptorPool, flags );
}

void Device::updateDescriptorSets( uint32_t                    descriptorWriteCount,
                                   const VkWriteDescriptorSet* pDescriptorWrites,
                                   uint32_t                    descriptorCopyCount,
                                   const VkCopyDescriptorSet*  pDescriptorCopies )
{
    vkUpdateDescriptorSets( this->id, descriptorWriteCount, pDescriptorWrites,
                            descriptorCopyCount, pDescriptorCopies );
}

// Swapchain Methods

VkResult Device::createSwapchain( const VkSwapchainCreateInfoKHR* pCreateInfo,
                                  VkSwapchainKHR*                 pSwapchain )
{
    return vkCreateSwapchainKHR( this->id, pCreateInfo, nullptr, pSwapchain );
}

void Device::destroySwapchain( VkSwapchainKHR swapchain )
{
    vkDestroySwapchainKHR( this->id, swapchain, nullptr );
}

VkResult Device::createSharedSwapchains( uint32_t                        swapchainCount,
                                         const VkSwapchainCreateInfoKHR* pCreateInfos,
                                         VkSwapchainKHR*                 pSwapchains )
{
    return vkCreateSharedSwapchainsKHR( this->id, swapchainCount,
                                        pCreateInfos, nullptr, pSwapchains );
}

VkResult Device::getSwapchainImages( VkSwapchainKHR swapchain,
                                     uint32_t*      pSwapchainImageCount,
                                     VkImage*       pSwapchainImages )
{
    return vkGetSwapchainImagesKHR( this->id, swapchain,
                                    pSwapchainImageCount, pSwapchainImages );
}

VkResult Device::acquireNextImage( VkSwapchainKHR swapchain,
                                   uint64_t       timeout,
                                   VkSemaphore    semaphore,
                                   VkFence        fence,
                                   uint32_t*      pImageIndex )
{
    return vkAcquireNextImageKHR( this->id, swapchain, timeout,
                                  semaphore, fence, pImageIndex );
}

