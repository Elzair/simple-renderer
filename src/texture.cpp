#include "common.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "texture.hpp"

void Texture::init( Device*      device,
                    VkQueue      queue,
                    CommandPool* commandPool,
                    std::string  fileName )
{
    // Load image from file
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load( fileName.c_str(),
                                 &texWidth,
                                 &texHeight,
                                 &texChannels,
                                 STBI_rgb_alpha );
    assert( pixels );
    std::size_t imageSize = texWidth * texHeight * 4;

    // Create texture
    Image::init( device,
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

void Texture::deinit()
{
    if ( this->sampler != VK_NULL_HANDLE )
    {
        this->device->destroySampler( this->sampler );
    }

    Image::deinit();
}
    
void Texture::createSampler()
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

    VK_CHECK_RESULT( this->device->createSampler( &samplerInfo,
                                                  &this->sampler ) );
}
