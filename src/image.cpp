#include <cstring>
#include <stdexcept>
#include "image.hpp"

Image::Image( VkPhysicalDevice physical,
              VkDevice         device,
              VkQueue          queue,
              VkCommandPool    commandPool,
              uint32_t         width,
              uint32_t         height,
              VkFormat         format,
              ImageType        type,
              void*            data,
              VkDeviceSize     dataSize )
{
    this->init( physical,
                device,
                queue,
                commandPool,
                width,
                height,
                format,
                type,
                data,
                dataSize );
}

Image::Image(  ) { }

Image::~Image(  )
{
    this->deinit();
}

void Image::init( VkPhysicalDevice physical,
                  VkDevice         device,
                  VkQueue          queue,
                  VkCommandPool    commandPool,
                  uint32_t         width,
                  uint32_t         height,
                  VkFormat         format,
                  ImageType        type,
                  void*            data,
                  VkDeviceSize     dataSize )
{
    this->device      = device;
    this->queue       = queue;
    this->commandPool = commandPool;
    this->width       = width;
    this->height      = height;
    this->format      = format;
    this->type        = type;

    VkImageUsageFlags  usage;
    VkImageLayout      initialLayout, finalLayout;
    VkImageAspectFlags aspectFlags;
        
    if ( this->type == ImageType::COLOR )
    {
        usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT;
        initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        aspectFlags   = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    else if ( this->type == ImageType::DEPTH )
    {
        usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        finalLayout   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        aspectFlags   = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
        
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.format        = this->format;
    imageInfo.extent.width  = this->width;
    imageInfo.extent.height = this->height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage         = usage;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = initialLayout;

    if ( vkCreateImage( this->device,
                        &imageInfo,
                        nullptr,
                        &this->id ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create image!" );
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements( this->device,
                                  this->id,
                                  &memRequirements );

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType( physical,
                                                memRequirements.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    if ( vkAllocateMemory( this->device,
                           &allocInfo,
                           nullptr,
                           &this->memory ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to allocate image memory!" );
    }

    vkBindImageMemory( this->device, this->id, this->memory, 0 );

    if ( this->type == ImageType::COLOR )
    {
        VkImage        staging       = VK_NULL_HANDLE;
        VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
    
        VkImageCreateInfo stagingInfo = {};
        stagingInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        stagingInfo.imageType     = VK_IMAGE_TYPE_2D;
        stagingInfo.format        = this->format;
        stagingInfo.extent.width  = this->width;
        stagingInfo.extent.height = this->height;
        stagingInfo.extent.depth  = 1;
        stagingInfo.mipLevels     = 1;
        stagingInfo.arrayLayers   = 1;
        stagingInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
        stagingInfo.tiling        = VK_IMAGE_TILING_LINEAR;
        stagingInfo.usage         = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        stagingInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
        stagingInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

        if ( vkCreateImage( this->device,
                            &stagingInfo,
                            nullptr ,
                            &staging ) != VK_SUCCESS )
        {
            throw std::runtime_error( "Failed to create staging image!" );
        }
            
        VkMemoryRequirements stagingMemReqs;
        vkGetImageMemoryRequirements( this->device,
                                      staging,
                                      &stagingMemReqs );

        VkMemoryAllocateInfo saInfo = {};
        saInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        saInfo.allocationSize  = stagingMemReqs.size;
        saInfo.memoryTypeIndex = FindMemoryType( physical,
                                                 stagingMemReqs.memoryTypeBits,
                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

        if ( vkAllocateMemory( this->device,
                               &saInfo,
                               nullptr,
                               &stagingMemory ) != VK_SUCCESS )
        {
            throw std::runtime_error( "Failed to allocate memory for staging image!" );
        }

        vkBindImageMemory( this->device, staging, stagingMemory, 0 );

        // Copy pixel data to staging area
        void* stagingData;
        vkMapMemory( this->device, stagingMemory, 0,
                     dataSize, 0, &stagingData );
        std::memcpy( stagingData, data, (size_t)dataSize );
        vkUnmapMemory( this->device, stagingMemory );

        // Optimize image layouts
        this->transitionLayout( staging,
                                VK_IMAGE_LAYOUT_PREINITIALIZED,
                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );
        this->transitionLayout( this->id,
                                initialLayout,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
        initialLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        // Copy image from staging area to device memory
        this->copy( staging );
    }

    // Transition image to final layout
    this->transitionLayout( this->id, initialLayout, finalLayout );

    // Create Image View
    this->createView( aspectFlags );
}
void Image::deinit(  )
{
    if ( this->view != VK_NULL_HANDLE )
    {
        vkDestroyImageView( this->device, this->view, nullptr );
    }
    if ( this->memory != VK_NULL_HANDLE )
    {
        vkFreeMemory( this->device, this->memory, nullptr );
    }
    if ( this->id != VK_NULL_HANDLE )
    {
        vkDestroyImage( this->device, this->id, nullptr );
    }
}

void Image::createView( VkImageAspectFlags aspectFlags )
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = this->id;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = this->format;
    viewInfo.subresourceRange.aspectMask     = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    if ( vkCreateImageView( this->device, &viewInfo,
                            nullptr, &this->view ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create texture image view!" );
    }

}

void Image::transitionLayout( VkImage       image,
                              VkImageLayout oldLayout,
                              VkImageLayout newLayout )
{
    CommandBuffer commandBuffer( this->device,
                                 this->queue,
                                 this->commandPool );
    commandBuffer.begin( CommandBufferUsage::ONE_TIME );

    // Create memory barrier
    VkImageMemoryBarrier barrier = {};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = oldLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.srcAccessMask                   = 0; // TODO
    barrier.dstAccessMask                   = 0; // TODO

    // Create barrier masks
    if ( oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED &&
         newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL )
    {
        barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    }
    else if ( oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED &&
              newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
    {
        barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
              newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }
    else if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
              newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    else
    {
        throw std::invalid_argument( "Unsupported layout transition!" );
    }

    vkCmdPipelineBarrier( commandBuffer.id,
                          VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                          VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                          0,
                          0,
                          nullptr,
                          0,
                          nullptr,
                          1,
                          &barrier );

    commandBuffer.end();
    commandBuffer.submit();
}

void Image::copy( VkImage src )
{
    CommandBuffer commandBuffer( this->device, this->queue, this->commandPool );
    commandBuffer.begin( CommandBufferUsage::ONE_TIME );

    VkImageSubresourceLayers subResource = {};
    subResource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    subResource.baseArrayLayer = 0;
    subResource.mipLevel       = 0;
    subResource.layerCount     = 1;

    VkImageCopy region = {};
    region.srcSubresource = subResource;
    region.dstSubresource = subResource;
    region.srcOffset      = {0, 0, 0};
    region.dstOffset      = {0, 0, 0};
    region.extent.width   = this->width;
    region.extent.height  = this->height;
    region.extent.depth   = 1;

    vkCmdCopyImage( commandBuffer.id,
                    src,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    this->id,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1,
                    &region );

    commandBuffer.submit();
}
