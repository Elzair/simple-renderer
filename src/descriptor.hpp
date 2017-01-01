#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "common.hpp"
#include "buffer.hpp"
#include "device.hpp"
#include "image.hpp"
//#include "sampler.hpp"

class Buffer;
class Image;
class Sampler;

struct DescriptorSet;

struct DescriptorBinding
{
    friend class DescriptorSetLayout;
    friend class DescriptorPool;
    
public:
    
    uint32_t           binding = 0;
    DescriptorType     type    = DescriptorType::SAMPLER;
    uint32_t           count   = 0;
    VkShaderStageFlags stages  = VK_SHADER_STAGE_ALL;

    DescriptorBinding() {}

    DescriptorBinding( uint32_t           binding,
                       DescriptorType     type,
                       uint32_t           count,
                       VkShaderStageFlags stages )
    {
        this->init( binding, type, count, stages );
    }

    void init( uint32_t           binding,
               DescriptorType     type,
               uint32_t           count,
               VkShaderStageFlags stages );

    // DescriptorBinding( uint32_t           a_binding,
    //                    DescriptorType     a_type,
    //                    uint32_t           a_count,
    //                    VkShaderStageFlags a_stages ) :
    //     binding( a_binding ),
    //     type( a_type ),
    //     count( a_count ),
    //     stages( a_stages ),
    //     internalBinding( { .binding            = a_binding,
    //                        .descriptorType     = (VkDescriptorType)a_type,
    //                        .descriptorCount    = a_count,
    //                        .stageFlags         = a_stages,
    //                        .pImmutableSamplers = nullptr } )
    // {}

    // DescriptorBinding( const DescriptorBinding& b ) :
    //     binding( b.binding ),
    //     type( b.type ),
    //     count( b.count ),
    //     stages( b.stages ),
    //     internalBinding( b.internalBinding )
    // {}

    // DescriptorBinding( DescriptorBinding&& b ) noexcept :
    //     binding( std::move( b.binding ) ),
    //     type( std::move( b.type ) ),
    //     count( std::move( b.count ) ),
    //     stages( std::move( b.stages ) ),
    //     internalBinding( std::move( b.internalBinding ) )
    // {}

private:

    VkDescriptorSetLayoutBinding internalBinding;
};

class DescriptorSetLayout
{
    friend class DescriptorPool;
    friend class PipelineLayout;
    
public:

    DescriptorSetLayout() {}

    DescriptorSetLayout( Device*                        device,
                         std::vector<DescriptorBinding> bindings )
    {
        this->init( device, bindings );
    }

    DescriptorSetLayout( DescriptorSetLayout&& d ) noexcept :
        id( d.id ),
        device( d.device ),
        bindings( std::move( d.bindings ) )
    {}

    ~DescriptorSetLayout()
    {
        this->deinit();
    }

    void init( Device*                         device,
               std::vector<DescriptorBinding>& bindings );

    void deinit();

    const std::vector<DescriptorBinding>& getBindings() const;

private:

    VkDescriptorSetLayout          id     = VK_NULL_HANDLE;
    Device*                        device = nullptr;
    std::vector<DescriptorBinding> bindings;
};

class DescriptorPool
{
    friend class DescriptorSet;
    
public:

    DescriptorPool() {}

    DescriptorPool( Device*              device,
                    DescriptorSetLayout* layout,
                    uint32_t             maxSets )
    {
        this->init( device, layout, maxSets );
    }

    ~DescriptorPool(  )
    {
        this->deinit();
    }

    void init( Device*              device,
               DescriptorSetLayout* layout,
               uint32_t             maxSets );

    void deinit();

    DescriptorSet allocateDescriptorSet();

private:

    Device*              device  = nullptr;
    VkDescriptorPool     id      = VK_NULL_HANDLE;
    DescriptorSetLayout* layout  = nullptr;
    uint32_t             maxSets = 0;
};

struct DescriptorSet
{
    friend class CommandBuffer;
    friend class DescriptorPool;
    
public:
    
    DescriptorSet() {}

    DescriptorType getType( uint32_t binding );

    void update( const Buffer& buffer,
                 uint32_t      binding,
                 uint32_t      arrayElement,
                 std::size_t   offset = 0,
                 std::size_t   range  = 0 );

    void update( const Image&   image,
                 const Sampler& sampler,
                 uint32_t       binding,
                 uint32_t       arrayElement );
    
private:

    void init( VkDescriptorSet id,
               DescriptorPool* pool );

    VkDescriptorSet id   = VK_NULL_HANDLE;
    DescriptorPool* pool = nullptr;
};

struct PipelineLayout
{
    friend class CommandBuffer;
    friend class GraphicsPipeline;
    
public:

    PipelineLayout() {}

    PipelineLayout( Device*                                 device,
                    const std::vector<DescriptorSetLayout>& layouts )
    {
        this->init( device, layouts );
    }

    ~PipelineLayout()
    {
        this->deinit();
    }

    void init( Device*                                 device,
               const std::vector<DescriptorSetLayout>& layouts );

    void deinit();
    
private:

    Device*          device = nullptr;
    VkPipelineLayout id     = VK_NULL_HANDLE;
};
