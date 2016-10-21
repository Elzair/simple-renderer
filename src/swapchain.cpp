#include <stdexcept>

#include "swapchain.hpp"
    
SwapChain::SwapChain( VkPhysicalDevice      physical,
                      VkDevice              device,
                      VkSurfaceKHR          surface,
                      int                   width,
                      int                   height,
                      std::vector<uint32_t> familyIndices)
{
    this->init( physical, device, surface,
                width, height, familyIndices );
}

SwapChain::SwapChain(  ) { }

SwapChain::~SwapChain(  )
{
    this->deinit();
}

void SwapChain::init( VkPhysicalDevice      physical,
                      VkDevice              device,
                      VkSurfaceKHR          surface,
                      int                   width,
                      int                   height,
                      std::vector<uint32_t> familyIndices )
{
    this->physical = physical;
    this->device   = device;
    this->surface  = surface;

    auto supported     = QuerySwapChainSupport( this->physical, this->surface );
    auto surfaceFormat = chooseSurfaceFormat( supported.formats );
    auto presentMode   = choosePresentMode( supported.presentModes );
    this->extent       = chooseExtent( supported.capabilities,
                                       width,
                                       height );
    this->imageFormat  = surfaceFormat.format;

    // Ensure we have enough images to properly implement triple buffering
    // A value of 0 for maxImageCount means there is no hard limit on # images
    uint32_t imageCount = supported.capabilities.minImageCount + 1;
    if ( supported.capabilities.maxImageCount > 0 &&
         imageCount > supported.capabilities.maxImageCount )
    {
        imageCount = supported.capabilities.maxImageCount;
    }

    // Create struct used to create a swapchain
    VkSwapchainKHR oldSwapchain = this->id;
    
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = this->surface;
    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = this->imageFormat;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = this->extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if ( familyIndices.size() >= 2 &&
         familyIndices[0] != familyIndices[1] )
    {
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = familyIndices.size();
        createInfo.pQueueFamilyIndices   = familyIndices.data();
    }
    else
    {
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices   = nullptr;
    }
    createInfo.preTransform   = supported.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode    = presentMode;
    createInfo.clipped        = VK_TRUE;
    createInfo.oldSwapchain   = oldSwapchain;

    // Create swapchain
    VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;
    if ( vkCreateSwapchainKHR( this->device,
                               &createInfo,
                               nullptr,
                               &newSwapchain ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create swapchain!" );
    }
    *&this->id = newSwapchain;

    // Retrieve handles to all swapchain images
    // Since the implementation can create more images,
    // we must query the number of images
    vkGetSwapchainImagesKHR( this->device, this->id,
                             &imageCount, nullptr );
    this->images.resize( imageCount );
    vkGetSwapchainImagesKHR( this->device, this->id,
                             &imageCount, this->images.data() );

    // Create Image Views
    this->imageViews.resize( this->images.size() );

    for ( uint32_t i = 0; i < this->images.size(); i++ )
    {
        this->imageViews[i] = this->createImageView( this->images[i],
                                                     VK_IMAGE_ASPECT_COLOR_BIT );
    }

    this->initialized = true;
}
    
void SwapChain::deinit( bool destroySwapchain )
{
    for ( auto fb : this->framebuffers )
    {
        vkDestroyFramebuffer( this->device, fb, nullptr );
    }
    for ( auto imgview : this->imageViews )
    {
        vkDestroyImageView( this->device, imgview, nullptr );
    }
    if ( destroySwapchain && this->id != VK_NULL_HANDLE )
    {
        vkDestroySwapchainKHR( this->device, this->id, nullptr );
    }

    this->initialized = ( this->id == VK_NULL_HANDLE ) ? false : true;
}

void SwapChain::refresh( VkPhysicalDevice      physical,
                         VkDevice              device,
                         VkSurfaceKHR          surface,
                         int                   width,
                         int                   height,
                         std::vector<uint32_t> familyIndices )
{
    this->deinit( false );
    this->init( physical, device, surface, width, height, familyIndices );
}

void SwapChain::createFramebuffers( VkRenderPass renderPass,
                                    Image*       images,
                                    std::size_t  numImages )
{
    this->framebuffers.resize( this->imageViews.size() );

    // Create attachments vector for VkImageViews
    std::vector<VkImageView> attachments( numImages + 1 );
    for ( std::size_t i = 0; i < numImages; i++ )
    {
        attachments[i+1] = images[i].view;
    }

    // Create framebuffers for image views
    for ( size_t i = 0; i < this->imageViews.size(); i++ )
    {
        attachments[0] = this->imageViews[i];

        VkFramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass      = renderPass;
        framebufferCreateInfo.attachmentCount = attachments.size();
        framebufferCreateInfo.pAttachments    = attachments.data();
        framebufferCreateInfo.width           = this->extent.width;
        framebufferCreateInfo.height          = this->extent.height;
        framebufferCreateInfo.layers          = 1;

        if ( vkCreateFramebuffer( this->device,
                                  &framebufferCreateInfo,
                                  nullptr,
                                  &this->framebuffers[i] ) != VK_SUCCESS )
        {
            throw std::runtime_error( "Failed to create framebuffer!" );
        }
    }
}


VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats
    )
{
    // First, check if surface has no preferred format
    if ( availableFormats.size() == 1 &&
         availableFormats[0].format == VK_FORMAT_UNDEFINED )
    {
        return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    }

    // Next, check if our preferred format is in the list
    for ( const auto& format : availableFormats )
    {
        if ( format.format == VK_FORMAT_B8G8R8A8_UNORM &&
             format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR )
        {
            return format;
        }
    }

    // Otherwise, default to first format
    return availableFormats[ 0 ];
}

VkPresentModeKHR SwapChain::choosePresentMode(
    const std::vector<VkPresentModeKHR> availablePresentModes
    )
{
    for ( const auto& presentMode : availablePresentModes )
    {
        if ( presentMode == VK_PRESENT_MODE_MAILBOX_KHR )
        {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR; // Guranteed to be available
}

VkExtent2D SwapChain::chooseExtent(
    const VkSurfaceCapabilitiesKHR& capabilities,
    int                             width,
    int                             height )
{
    if ( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = { (uint32_t)width, (uint32_t)height };
        actualExtent.width = std::max( capabilities.minImageExtent.width,
                                       std::min( capabilities.maxImageExtent.width,
                                                 actualExtent.width ) );
        actualExtent.height = std::max( capabilities.minImageExtent.height,
                                        std::min( capabilities.maxImageExtent.height,
                                                  actualExtent.height ) );

        return actualExtent;
    }
}

VkImageView SwapChain::createImageView( VkImage            image,
                                        VkImageAspectFlags aspectFlags )
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = image;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = this->imageFormat;
    viewInfo.subresourceRange.aspectMask     = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    VkImageView view = VK_NULL_HANDLE;

    if ( vkCreateImageView( this->device, &viewInfo,
                            nullptr, &view ) != VK_SUCCESS )
    {
        throw std::runtime_error( "Failed to create texture image view!" );
    }

    return view;
}
