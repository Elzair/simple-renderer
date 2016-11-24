#pragma once

#include <vulkan/vulkan.h>

#include "utils.hpp"
#include "device.hpp"
#include "commandbuffer.hpp"

enum ImageType {
    COLOR,
    DEPTH
};

class Image 
{
public:

    VkImage        id     = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView    view   = VK_NULL_HANDLE;

    Image( VkPhysicalDevice physical,
           Device*          device,
           VkQueue          queue,
           VkCommandPool    commandPool,
           uint32_t         width,
           uint32_t         height,
           VkFormat         format,
           ImageType        type,
           void*            data     = nullptr,
           VkDeviceSize     dataSize = 0 )
    {
        this->init( physical, device, queue, commandPool, width,
                    height, format, type, data, dataSize );
    }

    Image() {}

    ~Image() { this->deinit(); }

    void init( VkPhysicalDevice physical,
               Device*          device,
               VkQueue          queue,
               VkCommandPool    commandPool,
               uint32_t         width,
               uint32_t         height,
               VkFormat         format,
               ImageType        type,
               void*            data     = nullptr,
               VkDeviceSize     dataSize = 0 );

    void deinit(  );

protected:

    Device*        device;
    VkQueue        queue       = VK_NULL_HANDLE;
    VkCommandPool  commandPool = VK_NULL_HANDLE;
    uint32_t       width;
    uint32_t       height;
    VkFormat       format;
    ImageType      type;

    void createView( VkImageAspectFlags aspectFlags );

    void transitionLayout( VkImage       image,
                           VkImageLayout oldLayout,
                           VkImageLayout newLayout );

    void copy( VkImage src );
};
