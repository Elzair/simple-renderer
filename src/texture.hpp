#pragma once

#include <string>

#include <vulkan/vulkan.h>

#include "image.hpp"

class Texture : public Image
{
public:

    VkSampler sampler = VK_NULL_HANDLE;

    Texture( VkPhysicalDevice physical,
             VkDevice         device,
             VkQueue          queue,
             VkCommandPool    commandPool,
             std::string      fileName )
    {
        this->init( physical, device, queue, commandPool, fileName );
    }

    Texture() {}

    ~Texture() { this->deinit(); }

    void init( VkPhysicalDevice physical,
               VkDevice         device,
               VkQueue          queue,
               VkCommandPool    commandPool,
               std::string      fileName );

    void deinit();

private:
    
    void createSampler();
};
