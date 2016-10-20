#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include "base-includes.hpp"
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

    Texture(  ) { }

    ~Texture(  )
    {
        this->deinit();
    }

    void init( VkPhysicalDevice physical,
               VkDevice         device,
               VkQueue          queue,
               VkCommandPool    commandPool,
               std::string      fileName )
    {
         // Load image from file
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels        = stbi_load( fileName.c_str(),
                                            &texWidth,
                                            &texHeight,
                                            &texChannels,
                                            STBI_rgb_alpha );
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if ( !pixels )
        {
            throw std::runtime_error( "Failed to load texture image!" );
        }

        // Create texture
        Image::init( physical,
                     device,
                     queue,
                     commandPool,
                     texWidth,
                     texHeight,
                     VK_FORMAT_R8G8B8A8_UNORM,
                     ImageType::COLOR,
                     (void*)pixels,
                     imageSize );

        stbi_image_free( pixels );  // Free file data

        this->createSampler();
    }

    void deinit(  )
    {
        if ( this->sampler != VK_NULL_HANDLE )
        {
            vkDestroySampler( this->device, this->sampler, nullptr );
        }
        Image::deinit();
    }

private:
    
    void createSampler(  )
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter               = VK_FILTER_LINEAR;
        samplerInfo.minFilter               = VK_FILTER_LINEAR;
        samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable        = VK_TRUE;
        samplerInfo.maxAnisotropy           = 16;
        samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK ;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable           = VK_FALSE;
        samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias              = 0.0f;
        samplerInfo.minLod                  = 0.0f;
        samplerInfo.maxLod                  = 0.0f;
    
        if ( vkCreateSampler( this->device,
                              &samplerInfo,
                              nullptr,
                              &this->sampler ) != VK_SUCCESS )
        {
            throw std::runtime_error( "Failed to create texture sampler!" );
        }
    }
};

#endif
