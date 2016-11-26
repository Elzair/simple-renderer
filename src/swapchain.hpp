#pragma once

#include <limits>

#include <vector>

#include <vulkan/vulkan.h>

#include "image.hpp"
#include "utils.hpp"

class SwapChain
{
public:

    VkSwapchainKHR             id = VK_NULL_HANDLE;
    std::vector<VkImage>       images;
    VkFormat                   imageFormat;
    VkExtent2D                 extent;
    std::vector<VkImageView>   imageViews;
    std::vector<VkFramebuffer> framebuffers;
    
    SwapChain( VkPhysicalDevice      physical,
               Device*               device,
               VkSurfaceKHR          surface,
               int                   width,
               int                   height,
               std::vector<uint32_t> familyIndices )
    {
        this->init( physical, device, surface, width, height, familyIndices );
    }

    SwapChain() {}

    ~SwapChain() { this->deinit( true ); }

    void init( VkPhysicalDevice      physical,
               Device*               device,
               VkSurfaceKHR          surface,
               int                   width,
               int                   height,
               std::vector<uint32_t> familyIndices );
    
    void deinit( bool destroySwapchain = true );

    void refresh( VkPhysicalDevice      physical,
                  Device*               device,
                  VkSurfaceKHR          surface,
                  int                   width,
                  int                   height,
                  std::vector<uint32_t> familyIndices );

    void createFramebuffers( VkRenderPass renderPass,
                             Image*       images,
                             std::size_t  numImages );

private:

    VkPhysicalDevice physical;
    Device*          device;
    VkSurfaceKHR     surface;
    bool             initialized = false;

    VkSurfaceFormatKHR chooseSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats
        );

    VkPresentModeKHR choosePresentMode(
        const std::vector<VkPresentModeKHR> availablePresentModes
        );

    VkExtent2D chooseExtent( const VkSurfaceCapabilitiesKHR& capabilities,
                             int                             width,
                             int                             height );
    

    VkImageView createImageView( VkImage            image,
                                 VkImageAspectFlags aspectFlags );
};
