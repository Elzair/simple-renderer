#include "common.hpp"
#include "descriptor.hpp"
#include <cassert>

void DescriptorBinding::init( uint32_t           binding,
                              DescriptorType     type,
                              uint32_t           count,
                              VkShaderStageFlags stages )
{
    this->binding = binding;
    this->type    = type;
    this->count   = count;
    this->stages  = stages;

    this->internalBinding.binding            = binding;
    this->internalBinding.descriptorType     = (VkDescriptorType)type;
    this->internalBinding.descriptorCount    = count;
    this->internalBinding.stageFlags         = stages;
    this->internalBinding.pImmutableSamplers = nullptr;
}

void DescriptorSetLayout::init(
    Device*                         device,
    std::vector<DescriptorBinding>& bindings
    )
{
    this->device   = device;
    this->bindings = bindings;
    
    std::vector<VkDescriptorSetLayoutBinding> tmpBindings;
    tmpBindings.reserve( this->bindings.size() );

    for ( auto& binding : this->bindings )
    {
        tmpBindings.emplace_back( binding.internalBinding );
    }
    
    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = tmpBindings.size();
    createInfo.pBindings    = tmpBindings.data();

    VK_CHECK_RESULT( this->device->createDescriptorSetLayout( &createInfo,
                                                              &this->id ) );
}

void DescriptorSetLayout::deinit()
{
    if ( this->id != VK_NULL_HANDLE )
    {
        this->device->destroyDescriptorSetLayout( this->id );
    }
}

const std::vector<DescriptorBinding>&
DescriptorSetLayout::getBindings() const
{
    return this->bindings;
}

void DescriptorPool::init( Device*              device,
                           DescriptorSetLayout* layout,
                           uint32_t             maxSets )
{
    this->device  = device;
    this->layout  = layout;
    this->maxSets = maxSets;

    std::vector<VkDescriptorPoolSize> poolSizes;

    // Merge descriptors of equal type into one pool size struct..
    for ( auto& binding : this->layout->bindings )
    {
        bool found = false;
            
        for ( auto& psiz : poolSizes )
        {
            if ( psiz.type == binding.internalBinding.descriptorType )
            {
                found = true;
                psiz.descriptorCount += binding.internalBinding.descriptorCount;
            }
        }

        if ( !found )
        {
            VkDescriptorPoolSize tmp = {
                .type            = binding.internalBinding.descriptorType,
                .descriptorCount = binding.internalBinding.descriptorCount
            };
            poolSizes.emplace_back( tmp );
        }
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = this->maxSets;

    VK_CHECK_RESULT( this->device->createDescriptorPool( &poolInfo,
                                                         &this->id ) );
}

void DescriptorPool::deinit()
{
    if ( this->id != VK_NULL_HANDLE )
    {
        this->device->destroyDescriptorPool( this->id );
    }
}

DescriptorSet DescriptorPool::allocateDescriptorSet()
{
    VkDescriptorSetAllocateInfo info = {};
    info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool     = this->id;
    info.descriptorSetCount = 1; // Currently, each DescriptorSetLayout must have its own DescriptorPool.
    info.pSetLayouts        = &this->layout->id;

    DescriptorSet   descriptorSet;
    VkDescriptorSet dsetID;

    VK_CHECK_RESULT( this->device->allocateDescriptorSets( &info,
                                                           &dsetID ) );

    descriptorSet.init( dsetID, this );

    return descriptorSet;
}

void DescriptorSet::init( VkDescriptorSet id,
                          DescriptorPool* pool )
{
    this->id   = id;
    this->pool = pool;
}

DescriptorType DescriptorSet::getType( uint32_t binding )
{
    for ( auto& set : this->pool->layout->getBindings() )
    {
        if ( set.binding == binding )
        {
            return set.type;
        }
    }

    assert( 0 );
}

void DescriptorSet::update( const Buffer& buffer,
                            uint32_t      binding,
                            uint32_t      arrayElement,
                            std::size_t   offset,
                            std::size_t   range)
{
    VkDescriptorBufferInfo info;
    info.buffer = buffer.id;
    info.offset = (VkDeviceSize) offset;
    info.range  = (VkDeviceSize)( range > 0
                                  ? (VkDeviceSize)range
                                  : ( buffer.size - offset ) );

    auto type = this->getType( binding );

    VkWriteDescriptorSet write;
    write.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext            = nullptr;
    write.dstSet           = this->id;
    write.dstBinding       = binding;
    write.dstArrayElement  = arrayElement;
    write.descriptorCount  = 1;
    write.descriptorType   = (VkDescriptorType)type;
    write.pBufferInfo      = &info;
    write.pImageInfo       = nullptr;
    write.pTexelBufferView = nullptr;

    this->pool->device->updateDescriptorSets( 1,
                                              &write,
                                              0,
                                              nullptr );
}

void DescriptorSet::update( const Image&   image,
                            const Sampler& sampler,
                            uint32_t       binding,
                            uint32_t       arrayElement )
{
    VkDescriptorImageInfo info;
    info.imageLayout = image.layout;
    info.imageView   = image.view;
    info.sampler     = sampler.id;

    auto type = this->getType( binding );

    VkWriteDescriptorSet write;
    write.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext            = nullptr;
    write.dstSet           = this->id;
    write.dstBinding       = binding;
    write.dstArrayElement  = arrayElement;
    write.descriptorCount  = 1;
    write.descriptorType   = (VkDescriptorType)type;
    write.pBufferInfo      = nullptr;
    write.pImageInfo       = &info;
    write.pTexelBufferView = nullptr;

    this->pool->device->updateDescriptorSets( 1,
                                              &write,
                                              0,
                                              nullptr );
}

void PipelineLayout::init( Device*                                 device,
                           const std::vector<DescriptorSetLayout>& layouts )
{
    this->device = device;
    
    std::vector<VkDescriptorSetLayout> internalLayouts;
    internalLayouts.reserve( layouts.size() );

    for ( auto& layout : layouts )
    {
        internalLayouts.emplace_back( layout.id );
    }

    VkPipelineLayoutCreateInfo info;
    info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.pNext                  = nullptr;
    info.flags                  = 0;
    info.setLayoutCount         = internalLayouts.size();
    info.pSetLayouts            = internalLayouts.data();
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges    = nullptr;

    VK_CHECK_RESULT( this->device->createPipelineLayout( &info, &this->id ) );
}

void PipelineLayout::deinit()
{
    if ( this->id != VK_NULL_HANDLE )
    {
        this->device->destroyPipelineLayout( this->id );
    }
}
