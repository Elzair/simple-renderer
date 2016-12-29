#pragma once

#include <string>

#include <vulkan/vulkan.h>

#include "device.hpp"
#include "image.hpp"

class Texture : public Image
{
public:

    VkSampler sampler = VK_NULL_HANDLE;

    Texture( Device*      device,
             VkQueue      queue,
             CommandPool* commandPool,
             std::string  fileName )
    {
        this->init( device, queue, commandPool, fileName );
    }

    Texture() {}

    ~Texture() { this->deinit(); }

    void init( Device*      device,
               VkQueue      queue,
               CommandPool* commandPool,
               std::string  fileName );

    void deinit();

private:
    
    void createSampler();
};
