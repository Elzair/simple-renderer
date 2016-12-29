#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "device.hpp"

struct ResourceBinding
{
    uint32_t           binding = 0;
    VkDescriptorType   type    = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    uint32_t           count   = 0;
    VkShaderStageFlags stages  = VK_SHADER_STAGE_ALL;

    ResourceBinding() {}

    ResourceBinding( uint32_t           a_binding,
                     VkDescriptorType   a_type,
                     uint32_t           a_count,
                     VkShaderStageFlags a_stages ) :
        binding( a_binding ), type( a_type ), count( a_count ), stages( a_stages ) {}

    ResourceBinding( const ResourceBinding& b ) :
        binding( b.binding ), type( b.type ), count( b.count ), stages( b.stages ) {}
};

struct ResourceInfo
{
    std::vector<ResourceBinding> bindings;

    ResourceInfo() {}

    ResourceInfo( std::vector<ResourceBinding> a_bindings ) :
        bindings( std::move( a_bindings ) ) {}

    ResourceInfo( const ResourceInfo& d ) :
        bindings( std::move( d.bindings ) ) {}
};

class ResourceLayoutContainer
{
    friend class CommandBuffer;

public:

    ResourceLayoutContainer() {}

    ResourceLayoutContainer( Device*                   device,
                             std::vector<ResourceInfo> info )
    {
        this->init( device, info );
    }

    ~ResourceLayoutContainer() { this->deinit(); }

    void init( Device*                   device,
               std::vector<ResourceInfo> info  );

    void deinit();

    std::size_t getNumLayouts() const;

    VkDescriptorSetLayout getLayout( std::size_t idx ) const;

    const std::vector<ResourceBinding>& getBindings( std::size_t idx ) const;

    const std::vector<VkDescriptorSetLayout>& getLayouts() const;

    VkPipelineLayout getPipelineLayout() const;

private:

    Device*          device         = nullptr;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    
    std::vector<std::vector<ResourceBinding>> bindings;
    std::vector<VkDescriptorSetLayout>        layouts;

    void createPipelineLayout();
};

class ResourcePool
{
public:

    ResourcePool() {}

    ResourcePool( Device*                    device,
                  uint32_t                   maxSets,
                  ResourceLayoutContainer* layout )
    {
        this->init( device, maxSets, layout );
    }

    ~ResourcePool(  ) { this->deinit(); }

    void init( Device*                    device,
               uint32_t                   maxSets,
               ResourceLayoutContainer* layout );

    void deinit();

    uint32_t getId() const;

    uint32_t getMaxSets() const;

    VkDescriptorSet  allocateDescriptorSet();

private:

    Device*                    device  = nullptr;
    ResourceLayoutContainer* layout  = nullptr;
    uint32_t                   id      = 0;
    uint32_t                   maxSets = 0;
    VkDescriptorPool           pool    = VK_NULL_HANDLE;
};
