#pragma once

#include "common.hpp"
#include "device.hpp"

class RenderPass
{
    friend class CommandBuffer;
    
public:
    
    RenderPass() {} 

    RenderPass( Device* device, VkFormat imageFormat )
    {
        this->init( device, imageFormat );
    }

    ~RenderPass() { this->deinit(); }

    void init( Device* device, VkFormat imageFormat );

    void deinit();

    VkRenderPass getRenderPass() const;

private:

    Device*      device;
    VkRenderPass renderPass;
};
