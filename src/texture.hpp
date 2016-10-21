#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

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
             std::string      fileName );

    Texture(  );

    ~Texture(  );

    void init( VkPhysicalDevice physical,
               VkDevice         device,
               VkQueue          queue,
               VkCommandPool    commandPool,
               std::string      fileName );

    void deinit(  );

private:
    
    void createSampler(  );
};

#endif
