#include "application.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

/*
 * Public Methods
 */

void VulkanApplication::run( int width, int height )
{
    glfwInit();
    initVulkan( width, height );
    mainLoop();
}

VulkanApplication::~VulkanApplication()
{
    this->device.destroySemaphore( this->renderFinishSemaphore );
    this->device.destroySemaphore( this->imageAvailableSemaphore );
    this->descriptorPool.deinit();
    this->uniform.deinit();
    this->model.deinit();
    this->texture.deinit();
    this->depth.deinit();
    //this->device.destroyCommandPool( this->commandPool );
    this->commandPool.deinit();
    this->graphicsPipeline.deinit();
    this->descriptorSetLayout.deinit();
    this->renderPass.deinit();
    this->swapchain.deinit();
    vkDestroySurfaceKHR( this->instance.id, this->surface, nullptr );
    std::cout << "Got here!" << std::endl;
    this->device.deinit();
    DestroyDebugReportCallbackEXT( this->instance.id, this->callback, nullptr );
    this->instance.deinit();
    glfwDestroyWindow( this->window );
}

/*
 * Private Methods
 */

void VulkanApplication::onWindowResized( GLFWwindow* window,
                                         int         width,
                                         int         height )
{
    if ( width == 0 || height == 0 )
    {
        return;
    }

    VulkanApplication* app = reinterpret_cast<VulkanApplication*>(
        glfwGetWindowUserPointer( window )
        );
    app->recreateSwapChain( width, height );
}
    
void VulkanApplication::initVulkan( int width, int height )
{
    this->width  = width;
    this->height = height;

    this->instance.init( "Hello Triangle",
                         "No Engine",
                         GetRequiredExtensions( enableValidationLayers ),
                         requiredValidationLayers );
    std::cout << "Created instance!" << std::endl;

    this->createDebugCallback();

    this->createSurface();

    this->physical = PickPhysicalDevice( this->instance.id,
                                         this->surface,
                                         requiredDeviceExtensions );

    this->device.init( this->physical,
                       this->surface,
                       requiredDeviceExtensions,
                       requiredValidationLayers );

    this->swapchain.init( this->physical,
                          &this->device,
                          this->surface,
                          this->width,
                          this->height,
                          { (uint32_t)this->device.graphicsQueueIdx,
                                  (uint32_t)this->device.presentQueueIdx } );
    std::cout << "Created SwapChain!" << std::endl;

    this->createRenderPass();
    std::cout << "Created Render Pass!" << std::endl;

    this->createDescriptorSetLayout();
    std::cout << "Created Descriptor Layout" << std::endl;
    this->createGraphicsPipeline();
    std::cout << "Created Graphics Pipeline!" << std::endl;
        
    this->createCommandPool();
    std::cout << "Created Command Pool!" << std::endl;
        
    this->depth.init( this->physical,
                      &this->device,
                      this->device.graphicsQueue,
                      &this->commandPool,
                      this->swapchain.extent.width,
                      this->swapchain.extent.height,
                      FindDepthFormat( this->physical ),
                      ImageType::DEPTH );
    std::cout << "Created Depth Image!" << std::endl;

    this->swapchain.createFramebuffers( this->renderPass.getRenderPass(),
                                        &this->depth,
                                        1 );
    std::cout << "Created Framebuffer!" << std::endl;

    this->texture.init( this->physical,
                        &this->device,
                        this->device.graphicsQueue,
                        &this->commandPool,
                        TEXTURE_PATH );
    std::cout << "Created Texture!" << std::endl;

    this->model.init( this->physical,
                      &this->device,
                      this->device.graphicsQueue,
                      &this->commandPool,
                      MODEL_PATH );
    std::cout << "Loaded model!" << std::endl;

    this->uniform.init( this->physical,
                        &this->device,
                        this->device.graphicsQueue,
                        &this->commandPool,
                        sizeof(UniformBufferObject),
                        BufferUsage::UNIFORM );
    std::cout << "Created Uniform Buffer!" << std::endl;
        
    this->createDescriptorPool();
    this->createDescriptorSet();
    std::cout << "Created Descriptor Sets!" << std::endl;
        
    this->createCommandBuffers();
        
    this->createSemaphores();
}

void VulkanApplication::mainLoop()
{
    while ( !glfwWindowShouldClose( this->window ) )
    {
        glfwPollEvents();

        this->updateUniformBuffer();
        this->drawFrame();
    }

    // Wait for logical device to finish
    vkDeviceWaitIdle( this->device.id );
}

void VulkanApplication::recreateSwapChain( int width, int height )
{
    vkDeviceWaitIdle( this->device.id );
    this->width  = width;
    this->height = height;

    this->swapchain.refresh( this->physical,
                             &this->device,
                             this->surface,
                             this->width,
                             this->height,
                             { (uint32_t)this->device.graphicsQueueIdx,
                               (uint32_t)this->device.presentQueueIdx } );
    this->createRenderPass();

    this->createGraphicsPipeline();

    this->depth.init( this->physical,
                      &this->device,
                      this->device.graphicsQueue,
                      &this->commandPool,
                      this->swapchain.extent.width,
                      this->swapchain.extent.height,
                      FindDepthFormat( this->physical ),
                      ImageType::DEPTH );

    this->swapchain.createFramebuffers( this->renderPass.getRenderPass(),
                                        &this->depth,
                                        1 );

    this->createCommandBuffers();
}

void VulkanApplication::updateUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto  currentTime = std::chrono::high_resolution_clock::now();
    float time        = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - startTime
        ).count() / 1000.0f;

    float aspect = (float)this->swapchain.extent.width /
        (float)this->swapchain.extent.height;
    UniformBufferObject ubo = {};
    ubo.model       = glm::rotate( glm::mat4(),
                                   time * glm::radians( 90.0f ),
                                   glm::vec3( 0.0f, 0.0f, 1.0f ) );
    ubo.view        = glm::lookAt( glm::vec3( 2.0f, 2.0f, 2.0f ),
                                   glm::vec3( 0.0f, 0.0f, 0.0f ),
                                   glm::vec3( 0.0f, 0.0f, 1.0f ) );
    ubo.proj        = glm::perspective( glm::radians(45.0f), aspect,
                                        0.1f, 10.0f );
    ubo.proj[1][1] *= -1; // Flip y coord to deal with vulkan's coordinate system

    this->uniform.copy( (void*)&ubo, true, sizeof(ubo) );
}

void VulkanApplication::drawFrame()
{
    uint32_t imageIdx;
    auto result = vkAcquireNextImageKHR( this->device.id,
                                         this->swapchain.id,
                                         std::numeric_limits<uint64_t>::max(), // Disable timeout for image to become available
                                         this->imageAvailableSemaphore,
                                         VK_NULL_HANDLE,
                                         &imageIdx );
    if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR )
    {
        this->recreateSwapChain( this->width, this->height );
        return;
    }
    VK_CHECK_RESULT( result );

    // Submit command buffer
    VkSemaphore waitSemaphores[]      = { this->imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[]    = { this->renderFinishSemaphore };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = waitSemaphores;
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &this->commandBuffers[imageIdx].id;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    VK_CHECK_RESULT( vkQueueSubmit( this->device.graphicsQueue,
                                    1,
                                    &submitInfo,
                                    VK_NULL_HANDLE ) );

    // Submit result to swap chain
    VkSwapchainKHR swapchains[] = { this->swapchain.id };

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = swapchains;
    presentInfo.pImageIndices      = &imageIdx;
    presentInfo.pResults           = nullptr;

    vkQueuePresentKHR( this->device.presentQueue, &presentInfo );
}

void VulkanApplication::createSurface()
{
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    
    this->window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr );

    glfwSetWindowUserPointer( this->window, this );
    glfwSetWindowSizeCallback( this->window, VulkanApplication::onWindowResized );

    // Create surface context
    VK_CHECK_RESULT( glfwCreateWindowSurface( this->instance.id,
                                              this->window,
                                              nullptr,
                                              &this->surface ) );
}

void VulkanApplication::createRenderPass()
{
    this->renderPass.init( &this->device,
                           this->physical,
                           this->swapchain.imageFormat );
}

void VulkanApplication::createDescriptorSetLayout()
{
    std::vector<ResourceBinding> bindings;
    bindings.emplace_back( 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                           1, VK_SHADER_STAGE_VERTEX_BIT );
    bindings.emplace_back( 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                           1, VK_SHADER_STAGE_FRAGMENT_BIT ); 
    this->descriptorSetLayout.init( &this->device, { { bindings } } );
}

void VulkanApplication::createGraphicsPipeline(  )
{
    // Load shader
    auto vs_code = ReadFile( "shaders/vert.spv" );
    auto fs_code = ReadFile( "shaders/frag.spv" );
    GraphicsShader shader( &this->device, vs_code, fs_code, {}, {}, {} );

    // Describe the format of the input vertex data
    auto vertexInfo    = Vertex::getBindingDescription();
    auto attributeInfo = Vertex::getAttributeDescriptions();
    std::vector<VkVertexInputAttributeDescription> attributeInfo2;
    for ( auto& ai : attributeInfo )
    {
        attributeInfo2.emplace_back( ai );
    }

    this->graphicsPipeline.init( &this->device, &this->renderPass, &shader,
                                 &this->descriptorSetLayout, &this->swapchain,
                                 vertexInfo, attributeInfo2 );
}

void VulkanApplication::createCommandPool()
{
    //VkCommandPoolCreateInfo poolCreateInfo = {};
    //poolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    //poolCreateInfo.queueFamilyIndex = this->device.graphicsQueueIdx; 
    //poolCreateInfo.flags            = 0;

    //VK_CHECK_RESULT( vkCreateCommandPool( this->device.id,
    //                                      &poolCreateInfo,
    //                                      nullptr,
    //                                      &this->commandPool ) );
    this->commandPool.init( &this->device,
                            this->device.graphicsQueue,
                            this->device.graphicsQueueIdx );
}

void VulkanApplication::createDescriptorPool()
{
    this->descriptorPool.init( &this->device,
                               20,
                               &this->descriptorSetLayout );
}

void VulkanApplication::createDescriptorSet()
{
    this->descriptorSet = this->descriptorPool.allocateDescriptorSet();

    // Now create buffer to hold uniform data
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = this->uniform.id;
    bufferInfo.offset = 0;
    bufferInfo.range  = sizeof(UniformBufferObject);

    // Create image to hold texture data
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView   = this->texture.view;
    imageInfo.sampler     = this->texture.sampler;

    // Update descriptors
    std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
    
    descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet          = this->descriptorSet;
    descriptorWrites[0].dstBinding      = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo     = &bufferInfo;
    
    descriptorWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet          = this->descriptorSet;
    descriptorWrites[1].dstBinding      = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo      = &imageInfo;

    vkUpdateDescriptorSets( this->device.id,
                            descriptorWrites.size(),
                            descriptorWrites.data(),
                            0,
                            nullptr );
}

void VulkanApplication::createCommandBuffers()
{
    // Free old command buffers (if called from recreateSwapChain())
    //this->commandBuffers.refresh( &this->device,
    //                              this->device.graphicsQueue,
    //                              this->commandPool,
    //                              this->swapchain.framebuffers.size() );
    for ( auto& cmdbuf : this->commandBuffers )
    {
        cmdbuf.deinit();
        cmdbuf.init( &this->device,
                     this->device.graphicsQueue,
                     &this->commandPool );
    }

    for ( size_t i = 0; i < this->commandBuffers.size(); i++ )
    {
        this->commandBuffers[i].begin();
                  
        // Start Render Pass
        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
      
        VkRenderPassBeginInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassCreateInfo.renderPass        = this->renderPass.getRenderPass();
        renderPassCreateInfo.framebuffer       = this->swapchain.framebuffers[i];
        renderPassCreateInfo.renderArea.offset = {0, 0};
        renderPassCreateInfo.renderArea.extent = this->swapchain.extent;
        renderPassCreateInfo.clearValueCount   = clearValues.size();
        renderPassCreateInfo.pClearValues      = clearValues.data();

        vkCmdBeginRenderPass( this->commandBuffers[i].id,
                              &renderPassCreateInfo,
                              VK_SUBPASS_CONTENTS_INLINE );

        vkCmdBindPipeline( this->commandBuffers[i].id,
                           VK_PIPELINE_BIND_POINT_GRAPHICS,
                           this->graphicsPipeline.getPipeline() );

        // Bind vertex buffer
        VkBuffer     vertexBuffers[] = { this->model.vertexBuffer.id };
        VkDeviceSize offsets[]       = { 0 };
        vkCmdBindVertexBuffers( this->commandBuffers[i].id, 0, 1,
                                vertexBuffers, offsets );

        // Bind index buffer
        vkCmdBindIndexBuffer( this->commandBuffers[i].id,
                              this->model.indexBuffer.id,
                              0,
                              VK_INDEX_TYPE_UINT32 );

        // Bind uniform buffer(s)
        vkCmdBindDescriptorSets( this->commandBuffers[i].id,
                                 VK_PIPELINE_BIND_POINT_GRAPHICS,
                                 //this->pipelineLayout,
                                 this->descriptorSetLayout.getPipelineLayout(),
                                 0,
                                 1,
                                 &this->descriptorSet,
                                 0,
                                 nullptr );
      
        vkCmdDrawIndexed( this->commandBuffers[i].id,
                          this->model.indexSize,
                          1, 0, 0, 0 );

        vkCmdEndRenderPass( this->commandBuffers[i].id );

        this->commandBuffers[i].end();
    }
}

void VulkanApplication::createSemaphores()
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VK_CHECK_RESULT( vkCreateSemaphore( this->device.id,
                                        &semaphoreCreateInfo,
                                        nullptr,
                                        &this->imageAvailableSemaphore ) );
    VK_CHECK_RESULT( vkCreateSemaphore( this->device.id,
                                        &semaphoreCreateInfo,
                                        nullptr,
                                        &this->renderFinishSemaphore ) );
}

VkBool32 VulkanApplication::debugCallback(
    VkDebugReportFlagsEXT      flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t                   obj,
    std::size_t                loc,
    int32_t                    code,
    const char*                layerPrefix,
    const char*                msg,
    void*                      userData )
{
    std::cerr << "Validation layer: " << msg << std::endl;
    return VK_FALSE;
}

void VulkanApplication::createDebugCallback()
{
    if ( !enableValidationLayers )
    {
        return;
    }
    
    VkDebugReportCallbackCreateInfoEXT crinfo = {};
    crinfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    crinfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    crinfo.pfnCallback = this->debugCallback;

    VK_CHECK_RESULT( CreateDebugReportCallbackEXT( this->instance.id,
                                                   &crinfo,
                                                   nullptr,
                                                   &this->callback ) );
}
