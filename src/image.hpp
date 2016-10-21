#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <vulkan/vulkan.h>

//#include "base-includes.hpp"
#include "utils.hpp"
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
           VkDevice         device,
           VkQueue          queue,
           VkCommandPool    commandPool,
           uint32_t         width,
           uint32_t         height,
           VkFormat         format,
           ImageType        type,
           void*            data     = nullptr,
           VkDeviceSize     dataSize = 0 );

    Image(  );

    ~Image(  );

    void init( VkPhysicalDevice physical,
               VkDevice         device,
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

    VkDevice       device      = VK_NULL_HANDLE;
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

#endif
