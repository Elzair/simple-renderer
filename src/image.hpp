#pragma once

#include <vulkan/vulkan.h>

#include "utils.hpp"
#include "device.hpp"
#include "commandbuffer.hpp"

class CommandPool;

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

    Image( Device*      device,
           VkQueue      queue,
           CommandPool* commandPool,
           uint32_t     width,
           uint32_t     height,
           VkFormat     format,
           ImageType    type,
           void*        data     = nullptr,
           std::size_t  dataSize = 0 )
    {
        this->init( device, queue, commandPool, width,
                    height, format, type, data, dataSize );
    }

    Image() {}

    ~Image() { this->deinit(); }

    void init( Device*      device,
               VkQueue      queue,
               CommandPool* commandPool,
               uint32_t     width,
               uint32_t     height,
               VkFormat     format,
               ImageType    type,
               void*        data     = nullptr,
               std::size_t  dataSize = 0 );

    void deinit();

protected:

    Device*        device      = nullptr;
    VkQueue        queue       = VK_NULL_HANDLE;
    CommandPool*   commandPool = nullptr;
    uint32_t       width;
    uint32_t       height;
    VkFormat       format;
    ImageType      type;

    void createView( VkImageAspectFlags aspectFlags );

    void transitionLayout( VkImage       image,
                           VkImageLayout oldLayout,
                           VkImageLayout newLayout,
                           ImageType     type );

    void copy( VkImage src );
};
