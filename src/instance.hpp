#ifndef __INSTANCE_HPP__
#define __INSTANCE_HPP__

#include "base-includes.hpp"

class Instance
{
public:
    VkInstance id = VK_NULL_HANDLE;

    Instance( const std::string              applicationName,
              const std::string              engineName,
              const std::vector<const char*> extensions,
              const std::vector<const char*> validationLayers,
              uint32_t                       applicationVersion = 1,
              uint32_t                       engineVersion = 1,
              uint32_t                       apiVersion = VK_API_VERSION_1_0 )
    {
        this->init( applicationName,
                    engineName,
                    extensions,
                    validationLayers,
                    applicationVersion,
                    engineVersion,
                    apiVersion );
    }
    
    Instance(  ) { }

    ~Instance(  )
    {
        this->deinit();
    }

    void init( const std::string              applicationName,
               const std::string              engineName,
               const std::vector<const char*> extensions,
               const std::vector<const char*> validationLayers,
               uint32_t                       applicationVersion = 1,
               uint32_t                       engineVersion = 1,
               uint32_t                       apiVersion = VK_API_VERSION_1_0 )
    {
        VkApplicationInfo appInfo  = {};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = applicationName.c_str();
        appInfo.applicationVersion = applicationVersion;
        appInfo.pEngineName        = engineName.c_str();
        appInfo.engineVersion      = engineVersion;
        appInfo.apiVersion         = apiVersion;

        VkInstanceCreateInfo instCreateInfo = {};
        instCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instCreateInfo.pApplicationInfo        = &appInfo;
        instCreateInfo.enabledExtensionCount   = extensions.size();
        instCreateInfo.ppEnabledExtensionNames = extensions.data();
        if ( validationLayers.size() > 0 )
        {
            instCreateInfo.enabledLayerCount   = validationLayers.size();
            instCreateInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            instCreateInfo.enabledLayerCount = 0;
        }

        if ( vkCreateInstance( &instCreateInfo, nullptr, &this->id ) != VK_SUCCESS )
        {
            throw std::runtime_error( "Failed to create instance!" );
        }
    }

    void deinit(  )
    {
        if ( this->id != VK_NULL_HANDLE )
        {
            vkDestroyInstance( this->id, nullptr );
        }
    }

};

#endif
