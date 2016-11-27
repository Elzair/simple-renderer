#include "common.hpp"
#include "resource.hpp"
#include <cassert>

void DescriptorLayoutContainer::init(
    Device*                        device,
    std::vector<DescriptorSetInfo> info
    )
{
    this->device = device;
    this->layouts.resize( info.size() );
    this->bindings.resize( info.size() );

    std::size_t idx = 0;
    for ( auto& i : info )
    {
        this->bindings[ idx ] = i.bindings;

        std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
        layout_bindings.resize( i.bindings.size() );

        for ( auto j = 0; j < i.bindings.size(); j++ )
        {
            layout_bindings[ j ] = {};
            layout_bindings[ j ].binding         = i.bindings[ j ].binding;
            layout_bindings[ j ].descriptorType  = i.bindings[ j ].type;
            layout_bindings[ j ].descriptorCount = i.bindings[ j ].count;
            layout_bindings[ j ].stageFlags      = i.bindings[ j ].stages;
        }

        VkDescriptorSetLayoutCreateInfo dslcinfo = {};
        dslcinfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        dslcinfo.bindingCount = layout_bindings.size();
        dslcinfo.pBindings    = layout_bindings.data();

        VK_CHECK_RESULT( this->device->createDescriptorSetLayout( &dslcinfo,
                                                                  &this->layouts[ idx ] ) );
        idx++;
    }

    this->createPipelineLayout();
}

void DescriptorLayoutContainer::deinit()
{
    for ( auto& layout : this->layouts )
    {
        this->device->destroyDescriptorSetLayout( layout );
    }
}

std::size_t DescriptorLayoutContainer::getNumLayouts() const
{
    return this->layouts.size();
}

VkDescriptorSetLayout DescriptorLayoutContainer::getLayout( std::size_t idx ) const
{
    assert( ( idx >= 0 ) && ( idx < this->layouts.size() ) );
    return this->layouts[ idx ];
}

const std::vector<DescriptorBinding>& DescriptorLayoutContainer::getBindings( std::size_t idx ) const
{
    assert( ( idx >= 0 ) && ( idx < this->bindings.size() ) );
    return this->bindings[ idx ];
}

const std::vector<VkDescriptorSetLayout>& DescriptorLayoutContainer::getLayouts() const
{
    return this->layouts;
}
     
VkPipelineLayout DescriptorLayoutContainer::getPipelineLayout() const
{
    return this->pipelineLayout;
}

void DescriptorLayoutContainer::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo info = {};
    info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.setLayoutCount         = this->layouts.size();
    info.pSetLayouts            = &this->layouts[ 0 ];
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges    = 0;

    VK_CHECK_RESULT( this->device->createPipelineLayout( &info,
                                                         &this->pipelineLayout ) );
}

void DescriptorPool::init( Device*                    device,
                           uint32_t                   maxSets,
                           DescriptorLayoutContainer* layout )
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

    std::cout << "Creating descriptor pool: " << poolSizes.size() << std::endl;
    for ( auto& p : poolSizes )
    {
        std::cout << "Type: " << p.type << " Count: " << p.descriptorCount << std::endl;
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = this->maxSets;

    VK_CHECK_RESULT( device->createDescriptorPool( &poolInfo, &this->pool ) );
}

void DescriptorPool::deinit()
{
    if ( this->pool != VK_NULL_HANDLE )
    {
        this->device->destroyDescriptorPool( this->pool );
    }
}

uint32_t DescriptorPool::getId() const
{
    return this->id;
}

uint32_t DescriptorPool::getMaxSets() const
{
    return this->maxSets;
}

VkDescriptorSet DescriptorPool::allocateDescriptorSet()
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

// void ResourceManager::init( Device*                    device,
//                             std::vector<uint32_t>      ids,
//                             DescriptorLayoutContainer* layouts )
// {
//     this->device  = device;
//     this->layouts = layouts;

//     this->descriptorPools.resize( ids.size() );

//     // Allocate descriptor pools
//     for ( auto id : ids )
//     {
//         this->descriptorPools[ id ].init( this->device,
//                                           this->layouts->getLayout( ids[ id ] ) );
//     }
// }

// DescriptorSet ResourceManager::allocateDescriptorSet( uint32_t id, uint32_t layoutId )
// {
    
// }
