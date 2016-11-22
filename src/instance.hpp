#ifndef __INSTANCE_HPP__
#define __INSTANCE_HPP__

//#include "base-includes.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class Instance
{
public:
    VkInstance id = VK_NULL_HANDLE;

    Instance( const std::string              applicationName,
              const std::string              engineName,
              const std::vector<const char*> extensions,
              const std::vector<const char*> validationLayers,
              uint32_t                       applicationVersion = 1,
              uint32_t                       engineVersion      = 1,
              uint32_t                       apiVersion         = VK_API_VERSION_1_0 )
    {
        this->init( applicationName,
                    engineName,
                    extensions,
                    validationLayers,
                    applicationVersion,
                    engineVersion,
                    apiVersion );
    }
    
    Instance() {}

    ~Instance() { this->deinit(); }

    void init( const std::string              applicationName,
               const std::string              engineName,
               const std::vector<const char*> extensions,
               const std::vector<const char*> validationLayers,
               uint32_t                       applicationVersion = 1,
               uint32_t                       engineVersion = 1,
               uint32_t                       apiVersion = VK_API_VERSION_1_0 );

    void deinit();
};

#endif
