#pragma once

#include "common.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "resource.hpp"
#include "shader.hpp"
#include "swapchain.hpp"

struct ScreenDimensions
{
    uint32_t width;
    uint32_t height;
};

class GraphicsPipeline
{
public:

    GraphicsPipeline() {}

    GraphicsPipeline(
        Device*                                        device,
        RenderPass*                                    renderPass,
        GraphicsShader*                                shader,
        DescriptorLayoutContainer*                     layout,
        SwapChain*                                     swapchain,
        VkVertexInputBindingDescription                vertexInfo,
        std::vector<VkVertexInputAttributeDescription> attributeInfo
        )
    {
        this->init( device, renderPass, shader, layout,
                    swapchain, vertexInfo, attributeInfo );
    }

    ~GraphicsPipeline() { this->deinit(); }

    void init(
        Device*                                        device,
        RenderPass*                                    renderPass,
        GraphicsShader*                                shader,
        DescriptorLayoutContainer*                     layout,
        SwapChain*                                     swapchain,
        VkVertexInputBindingDescription                vertexInfo,
        std::vector<VkVertexInputAttributeDescription> attributeInfo
        );

    void deinit();

    VkPipeline getPipeline() const;

private:

    Device*         device     = nullptr;
    VkPipeline      pipeline   = VK_NULL_HANDLE;
};
