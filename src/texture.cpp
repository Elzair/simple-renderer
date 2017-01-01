#include "common.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "texture.hpp"

void Texture::init( Device*      device,
                    VkQueue      queue,
                    CommandPool* commandPool,
                    std::string  fileName )
{
    // Load image from file.
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load( fileName.c_str(),
                                 &texWidth,
                                 &texHeight,
                                 &texChannels,
                                 STBI_rgb_alpha );
    assert( pixels );
    std::size_t imageSize = texWidth * texHeight * 4;

    // Create texture.
    this->image.init( device,
                      queue,
                      commandPool,
                      texWidth,
                      texHeight,
                      VK_FORMAT_R8G8B8A8_UNORM,
                      ImageType::COLOR,
                      (void*)pixels,
                      imageSize );

    stbi_image_free( pixels );  // Free file data.

    this->sampler.init( device );
}

void Texture::deinit()
{
    this->sampler.deinit();
    this->image.deinit();
}

Image& Texture::getImage()
{
    return this->image;
}

Sampler& Texture::getSampler()
{
    return this->sampler;
}
