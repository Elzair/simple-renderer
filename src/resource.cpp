#include "common.hpp"
#include "resource.hpp"
#include <cassert>

void ResourceLayoutContainer::init(
    Device*                   device,
    std::vector<ResourceInfo> info
    )
{
    this->device = device;
    this->layouts.reserve( info.size() );
    this->bindings.reserve( info.size() );

    for ( auto& resource : info )
    {
        this->bindings.emplace_back( resource.bindings );

        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.reserve( resource.bindings.size() );

        for ( auto& binding : resource.bindings )
        {
            VkDescriptorSetLayoutBinding tmp;
            tmp.binding            = binding.binding;
            tmp.descriptorType     = binding.type;
            tmp.descriptorCount    = binding.count;
            tmp.stageFlags         = binding.stages;
            tmp.pImmutableSamplers = nullptr;

            layoutBindings.emplace_back( tmp );
        }

        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount = layoutBindings.size();
        createInfo.pBindings    = layoutBindings.data();

        VkDescriptorSetLayout layout;
        VK_CHECK_RESULT( this->device->createDescriptorSetLayout( &createInfo,
                                                                  &layout ) );
        this->layouts.emplace_back( layout );
    }

    this->createPipelineLayout();
}

void ResourceLayoutContainer::deinit()
{
    // Destroy pipeline layout
    if ( this->pipelineLayout != VK_NULL_HANDLE )
    {
        this->device->destroyPipelineLayout( this->pipelineLayout );
    }
    
    // Destroy descriptor layouts
    for ( auto& layout : this->layouts )
    {
        if ( layout != VK_NULL_HANDLE )
        {
            this->device->destroyDescriptorSetLayout( layout );
        }
    }
}

std::size_t ResourceLayoutContainer::getNumLayouts() const
{
    return this->layouts.size();
}

VkDescriptorSetLayout ResourceLayoutContainer::getLayout( std::size_t idx ) const
{
    assert( ( idx >= 0 ) && ( idx < this->layouts.size() ) );
    return this->layouts[ idx ];
}

const std::vector<ResourceBinding>& ResourceLayoutContainer::getBindings( std::size_t idx ) const
{
    assert( ( idx >= 0 ) && ( idx < this->bindings.size() ) );
    return this->bindings[ idx ];
}

const std::vector<VkDescriptorSetLayout>& ResourceLayoutContainer::getLayouts() const
{
    return this->layouts;
}
     
VkPipelineLayout ResourceLayoutContainer::getPipelineLayout() const
{
    return this->pipelineLayout;
}

void ResourceLayoutContainer::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo info = {};
    info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.setLayoutCount         = this->layouts.size();
    info.pSetLayouts            = this->layouts.data();
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges    = 0;

    VK_CHECK_RESULT( this->device->createPipelineLayout( &info,
                                                         &this->pipelineLayout ) );
}

void ResourcePool::init( Device*                  device,
                         uint32_t                 maxSets,
                         ResourceLayoutContainer* layout )
{
    this->device  = device;
    this->layout  = layout;
    this->maxSets = maxSets;

    std::vector<VkDescriptorPoolSize> poolSizes;

    std::size_t i, j, k;

    for ( i = 0; i < this->layout->getNumLayouts(); i++ )
    {
        auto layout   = this->layout->getLayout( i );
        auto bindings = this->layout->getBindings( i );

        for ( j = 0; j < bindings.size(); j++ )
        {
            auto type  = bindings[ j ].type;
            bool found = false;
            
            for ( k = 0; k < poolSizes.size(); k++ )
            {
                if ( poolSizes[ k ].type == type )
                {
                    found = true;
                    break;
                }
            }

            if ( found )
            {
                poolSizes[ k ].descriptorCount += bindings[ j ].count;
            }
            else
            {
                VkDescriptorPoolSize tmp = {};
                tmp.type            = bindings[ j ].type;
                tmp.descriptorCount = bindings[ j ].count;
                poolSizes.emplace_back( tmp );
            }
        }
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = this->maxSets;

    VK_CHECK_RESULT( device->createDescriptorPool( &poolInfo, &this->pool ) );
}

void ResourcePool::deinit()
{
    if ( this->pool != VK_NULL_HANDLE )
    {
        this->device->destroyDescriptorPool( this->pool );
    }
}

uint32_t ResourcePool::getId() const
{
    return this->id;
}

uint32_t ResourcePool::getMaxSets() const
{
    return this->maxSets;
}

VkDescriptorSet ResourcePool::allocateDescriptorSet()
{
    auto& tmp = this->layout->getLayouts();

    VkDescriptorSetAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool     = this->pool;
    info.descriptorSetCount = 1;
    info.pSetLayouts        = &tmp[ 0 ];

    VkDescriptorSet descriptorSet;

    VK_CHECK_RESULT( this->device->allocateDescriptorSets( &info,
                                                           &descriptorSet ) );

    return descriptorSet;
}
