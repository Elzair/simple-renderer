#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <cstring>
#include <chrono>
#include <functional>
#include <iostream>
#include <vector>

#include "buffer.hpp"
#include "common.hpp"
#include "device.hpp"
#include "image.hpp"
#include "instance.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "renderpass.hpp"
#include "resource.hpp"
#include "shader.hpp"
#include "swapchain.hpp"
#include "texture.hpp"
#include "ubo.hpp"
#include "utils.hpp"

class VulkanApplication
{
public:
    
    void run( int width, int height );

    ~VulkanApplication(  );

private:
    
    GLFWwindow*               window;
    int                       width;
    int                       height;

    Instance                  instance;
    VkDebugReportCallbackEXT  callback;
    VkSurfaceKHR              surface;

    VkPhysicalDevice          physical = VK_NULL_HANDLE;
    Device                    device;

    SwapChain                 swapchain;
   
    RenderPass                renderPass;

    DescriptorLayoutContainer descriptorSetLayout;

    //VkPipeline                graphicsPipeline = VK_NULL_HANDLE;
    GraphicsPipeline          graphicsPipeline;

    VkCommandPool             commandPool = VK_NULL_HANDLE;

    Image                     depth;

    Texture                   texture;

    Model                     model;

    Buffer                    uniform;

    DescriptorPool            descriptorPool;
    VkDescriptorSet           descriptorSet; // Freed when descriptorPool is destroyed

    CommandBuffers            commandBuffers;

    VkSemaphore               imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore               renderFinishSemaphore   = VK_NULL_HANDLE;

    static void onWindowResized( GLFWwindow* window,
                                 int         width,
                                 int         height );
    
    void initVulkan( int width, int height );

    void mainLoop();

    void recreateSwapChain( int width, int height );

    void updateUniformBuffer();

    void drawFrame();

    void createSurface();

    void createRenderPass();

    void createDescriptorSetLayout();

    void createGraphicsPipeline();

    void createCommandPool();

    void createDescriptorPool();

    void createDescriptorSet();

    void createCommandBuffers();

    void createSemaphores();

#ifndef WIN32
#define __stdcall
#endif
    static VkBool32 __stdcall debugCallback(
        VkDebugReportFlagsEXT      flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t                   obj,
        std::size_t                loc,
        int32_t                    code,
        const char*                layerPrefix,
        const char*                msg,
        void*                      userData
        );

    void createDebugCallback(  );
};
