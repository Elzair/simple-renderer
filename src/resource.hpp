#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "device.hpp"

struct DescriptorBinding
{
    uint32_t           binding = 0;
    VkDescriptorType   type    = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    uint32_t           count   = 0;
    VkShaderStageFlags stages  = VK_SHADER_STAGE_ALL;

    DescriptorBinding() {}

    DescriptorBinding( uint32_t           a_binding,
                       VkDescriptorType   a_type,
                       uint32_t           a_count,
                       VkShaderStageFlags a_stages ) :
        binding( a_binding ), type( a_type ), count( a_count ), stages( a_stages ) {}

    DescriptorBinding( const DescriptorBinding& b ) :
        binding( b.binding ), type( b.type ), count( b.count ), stages( b.stages ) {}
};

struct DescriptorSetInfo
{
    std::vector<DescriptorBinding> bindings;

    DescriptorSetInfo() {}

    DescriptorSetInfo( std::vector<DescriptorBinding> a_bindings ) :
        bindings( std::move( a_bindings ) ) {}

    DescriptorSetInfo( const DescriptorSetInfo& d ) :
        bindings( std::move( d.bindings ) ) {}
};

class DescriptorLayoutContainer
{
public:

    DescriptorLayoutContainer() {}

    DescriptorLayoutContainer( Device*                        device,
                               std::vector<DescriptorSetInfo> info )
    {
        this->init( device, info );
    }

    ~DescriptorLayoutContainer() { this->deinit(); }

    void init( Device*                        device,
               std::vector<DescriptorSetInfo> info  );

    void deinit();

    std::size_t getNumLayouts() const;

    VkDescriptorSetLayout getLayout( std::size_t idx ) const;

    const std::vector<DescriptorBinding>& getBindings( std::size_t idx ) const;

    const std::vector<VkDescriptorSetLayout>& getLayouts() const;

    VkPipelineLayout getPipelineLayout() const;

private:

    Device*          device         = nullptr;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    
    std::vector<std::vector<DescriptorBinding>> bindings;
    std::vector<VkDescriptorSetLayout>          layouts;

    void createPipelineLayout();
};

class DescriptorPool
{
public:

    DescriptorPool() {}

    DescriptorPool( Device*                    device,
                    uint32_t                   maxSets,
                    DescriptorLayoutContainer* layout )
    {
        this->init( device, maxSets, layout );
    }

    ~DescriptorPool(  ) { this->deinit(); }

    void init( Device*                    device,
               uint32_t                   maxSets,
               DescriptorLayoutContainer* layout );

    void deinit();

    uint32_t getId() const;

    uint32_t getMaxSets() const;

    VkDescriptorSet  allocateDescriptorSet();

private:

    Device*                    device  = nullptr;
    DescriptorLayoutContainer* layout  = nullptr;
    uint32_t                   id      = 0;
    uint32_t                   maxSets = 0;
    VkDescriptorPool           pool    = VK_NULL_HANDLE;
};
