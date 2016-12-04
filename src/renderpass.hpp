#pragma once

#include "common.hpp"
#include "device.hpp"

class RenderPass
{
    friend class CommandBuffer;
    
public:
    
    RenderPass() {} 

    RenderPass( Device* device, VkPhysicalDevice physical, VkFormat imageFormat )
    {
        this->init( device, physical, imageFormat );
    }

    ~RenderPass(  ) { this->deinit(); }

    void init( Device* device, VkPhysicalDevice physical, VkFormat imageFormat );

    void deinit();

    VkRenderPass getRenderPass() const;

private:

    Device*      device;
    VkRenderPass renderPass;
};
