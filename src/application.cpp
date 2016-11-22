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

VulkanApplication::~VulkanApplication(  )
{
    vkDestroySemaphore( this->device.id, this->renderFinishSemaphore, nullptr );
    vkDestroySemaphore( this->device.id, this->imageAvailableSemaphore, nullptr );
    vkDestroyDescriptorPool( this->device.id, this->descriptorPool, nullptr );
    this->uniform.deinit();
    this->model.deinit();
    this->texture.deinit();
    this->depth.deinit();
    vkDestroyCommandPool( this->device.id, this->commandPool, nullptr );
    vkDestroyPipeline( this->device.id, this->graphicsPipeline, nullptr );
    vkDestroyPipelineLayout( this->device.id, this->pipelineLayout, nullptr );
    vkDestroyDescriptorSetLayout( this->device.id, this->descriptorSetLayout, nullptr );
    vkDestroyRenderPass( this->device.id, this->renderPass, nullptr );
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
                          this->device.id,
                          this->surface,
                          this->width,
                          this->height,
                          { (uint32_t)this->device.graphicsQueueIdx,
                                  (uint32_t)this->device.presentQueueIdx } );
    std::cout << "Created SwapChain!" << std::endl;

    this->createRenderPass();
    std::cout << "Created Render Pass!" << std::endl;

    this->createDescriptorSetLayout();
    this->createGraphicsPipeline();
    std::cout << "Created Graphics Pipeline!" << std::endl;
        
    this->createCommandPool();
    std::cout << "Created Command Pool!" << std::endl;
        
    this->depth.init( this->physical,
                      this->device.id,
                      this->device.graphicsQueue,
                      this->commandPool,
                      this->swapchain.extent.width,
                      this->swapchain.extent.height,
                      FindDepthFormat( this->physical ),
                      ImageType::DEPTH );
    std::cout << "Created Depth Image!" << std::endl;

    this->swapchain.createFramebuffers( this->renderPass,
                                        &this->depth,
                                        1 );
    std::cout << "Created Framebuffer!" << std::endl;

    this->texture.init( this->physical,
                        this->device.id,
                        this->device.graphicsQueue,
                        this->commandPool,
                        TEXTURE_PATH );
    std::cout << "Created Texture!" << std::endl;

    this->model.init( this->physical,
                      this->device.id,
                      this->device.graphicsQueue,
                      this->commandPool,
                      MODEL_PATH );
    std::cout << "Loaded model!" << std::endl;

    this->uniform.init( this->physical,
                        this->device.id,
                        this->device.graphicsQueue,
                        this->commandPool,
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
                             this->device.id,
                             this->surface,
                             this->width,
                             this->height,
                             { (uint32_t)this->device.graphicsQueueIdx,
                                     (uint32_t)this->device.presentQueueIdx } );
    this->createRenderPass();

    this->createGraphicsPipeline();

    this->depth.init( this->physical,
                      this->device.id,
                      this->device.graphicsQueue,
                      this->commandPool,
                      this->swapchain.extent.width,
                      this->swapchain.extent.height,
                      FindDepthFormat( this->physical ),
                      ImageType::DEPTH );

    this->swapchain.createFramebuffers( this->renderPass,
                                        &this->depth,
                                        1 );

    this->createCommandBuffers();
}

void VulkanApplication::updateUniformBuffer(  )
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

void VulkanApplication::drawFrame(  )
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

void VulkanApplication::createSurface(  )
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
    // Add color buffer to render pass
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format         = this->swapchain.imageFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Add depth buffer to render pass
    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format         = FindDepthFormat( this->physical );
    depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDesc = {};
    subpassDesc.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.colorAttachmentCount    = 1;
    subpassDesc.pColorAttachments       = &colorAttachmentRef;
    subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;

    // Add subpass dependency
    VkSubpassDependency dependency = {};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {
        colorAttachment,
        depthAttachment
    };

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = attachments.size();
    renderPassCreateInfo.pAttachments    = attachments.data();
    renderPassCreateInfo.subpassCount    = 1;
    renderPassCreateInfo.pSubpasses      = &subpassDesc;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies   = &dependency;

    VK_CHECK_RESULT( vkCreateRenderPass( this->device.id, &renderPassCreateInfo,
                                         nullptr, &this->renderPass ) );
}

void VulkanApplication::createDescriptorSetLayout(  )
{
    // Creatout layout for uniform buffer
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding            = 0;
    uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount    = 1;
    uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    // Create layout for texture sampler
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding            = 1;
    samplerLayoutBinding.descriptorCount    = 1;
    samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
        uboLayoutBinding,
        samplerLayoutBinding
    };
    
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings    = bindings.data();

    VK_CHECK_RESULT( vkCreateDescriptorSetLayout( this->device.id,
                                                  &layoutInfo,
                                                  nullptr,
                                                  &this->descriptorSetLayout ) );
}

void VulkanApplication::createGraphicsPipeline(  )
{
    auto vs_code = ReadFile( "shaders/vert.spv" );
    auto fs_code = ReadFile( "shaders/frag.spv" );
    GraphicsShader shader( this->device.id, vs_code, fs_code, {}, {}, {} );

    // Describe the format of the input vertex data
    auto bindingDescription    = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    
    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
    vertexInputCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount   = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions      = &bindingDescription;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputCreateInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

    // Specify topology of input vertices
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
    inputAssemblyCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    // Create Viewport
    VkViewport viewport = {};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = (float) this->swapchain.extent.width;
    viewport.height   = (float) this->swapchain.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Create Scissor Rectangle
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = this->swapchain.extent;

    // Combine Viewport and Scissor Rectangle into Viewport State
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports    = &viewport;
    viewportStateCreateInfo.scissorCount  = 1;
    viewportStateCreateInfo.pScissors     = &scissor;

    // Create Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
    rasterizerCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerCreateInfo.depthClampEnable        = VK_FALSE;
    rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerCreateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizerCreateInfo.lineWidth               = 1.0f;
    rasterizerCreateInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizerCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizerCreateInfo.depthBiasEnable         = VK_FALSE;
    rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizerCreateInfo.depthBiasClamp          = 0.0f;
    rasterizerCreateInfo.depthBiasSlopeFactor    = 0.0f;

    // Create Depth Testing
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable       = VK_TRUE;
    depthStencil.depthWriteEnable      = VK_TRUE;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds        = 0.0f;
    depthStencil.maxDepthBounds        = 1.0f;
    depthStencil.stencilTestEnable     = VK_FALSE;
    depthStencil.front                 = {};
    depthStencil.back                  = {};

    // Create Multisampling
    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {};
    multisamplingCreateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingCreateInfo.sampleShadingEnable   = VK_FALSE;
    multisamplingCreateInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisamplingCreateInfo.minSampleShading      = 1.0f;
    multisamplingCreateInfo.pSampleMask           = nullptr;
    multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingCreateInfo.alphaToOneEnable      = VK_FALSE;

    // Configure Color Blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable         = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

    // Create Color Blending for all framebuffers
    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
    colorBlendCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreateInfo.logicOpEnable     = VK_FALSE;
    colorBlendCreateInfo.logicOp           = VK_LOGIC_OP_COPY;
    colorBlendCreateInfo.attachmentCount   = 1;
    colorBlendCreateInfo.pAttachments      = &colorBlendAttachment;
    colorBlendCreateInfo.blendConstants[0] = 0.0f;
    colorBlendCreateInfo.blendConstants[1] = 0.0f;
    colorBlendCreateInfo.blendConstants[2] = 0.0f;
    colorBlendCreateInfo.blendConstants[3] = 0.0f;

    // Create Pipeline Layout
    VkDescriptorSetLayout      setLayouts[]             = { this->descriptorSetLayout };
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount         = 1;
    pipelineLayoutCreateInfo.pSetLayouts            = setLayouts;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges    = 0;

    VK_CHECK_RESULT( vkCreatePipelineLayout( this->device.id,
                                             &pipelineLayoutCreateInfo,
                                             nullptr,
                                             &this->pipelineLayout ) );

    // Create Graphics Pipeline
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    //pipelineCreateInfo.stageCount          = 2;
    //pipelineCreateInfo.pStages             = shaderStages;
    pipelineCreateInfo.stageCount          = shader.num_modules;
    pipelineCreateInfo.pStages             = &shader.pipeline_info[ 0 ];
    pipelineCreateInfo.pVertexInputState   = &vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pViewportState      = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
    pipelineCreateInfo.pMultisampleState   = &multisamplingCreateInfo;
    pipelineCreateInfo.pDepthStencilState  = &depthStencil;
    pipelineCreateInfo.pColorBlendState    = &colorBlendCreateInfo;
    pipelineCreateInfo.pDynamicState       = nullptr;
    pipelineCreateInfo.layout              = this->pipelineLayout;
    pipelineCreateInfo.renderPass          = this->renderPass;
    pipelineCreateInfo.subpass             = 0;
    pipelineCreateInfo.basePipelineHandle  = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex   = -1;

    VK_CHECK_RESULT( vkCreateGraphicsPipelines( this->device.id,
                                                VK_NULL_HANDLE,
                                                1,
                                                &pipelineCreateInfo,
                                                nullptr,
                                                &this->graphicsPipeline ) );
}

void VulkanApplication::createCommandPool(  )
{
    VkCommandPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.queueFamilyIndex = this->device.graphicsQueueIdx; 
    poolCreateInfo.flags            = 0;

    VK_CHECK_RESULT( vkCreateCommandPool( this->device.id,
                                          &poolCreateInfo,
                                          nullptr,
                                          &this->commandPool ) );
}

void VulkanApplication::createDescriptorPool(  )
{
    std::array<VkDescriptorPoolSize, 2> poolSizes = {};
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = 1;
    
    VK_CHECK_RESULT( vkCreateDescriptorPool( this->device.id,
                                             &poolInfo,
                                             nullptr,
                                             &this->descriptorPool ) );
}

void VulkanApplication::createDescriptorSet(  )
{
    // Create initial descriptors
    VkDescriptorSetLayout       layouts[] = { this->descriptorSetLayout };
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = this->descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts        = layouts;

    VK_CHECK_RESULT( vkAllocateDescriptorSets( this->device.id,
                                               &allocInfo,
                                               &this->descriptorSet ) );

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

void VulkanApplication::createCommandBuffers(  )
{
    // Free old command buffers (if called from recreateSwapChain())
    this->commandBuffers.refresh( this->device.id,
                                  this->device.graphicsQueue,
                                  this->commandPool,
                                  this->swapchain.framebuffers.size() );

    for ( size_t i = 0; i < this->commandBuffers.size(); i++ )
    {
        this->commandBuffers[i].begin();
                  
        // Start Render Pass
        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
      
        VkRenderPassBeginInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassCreateInfo.renderPass        = this->renderPass;
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
                           this->graphicsPipeline );

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
                                 this->pipelineLayout,
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

void VulkanApplication::createSemaphores(  )
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
    void*                      userData)
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
