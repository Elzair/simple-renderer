#include "common.hpp"
#include "instance.hpp"

void Instance::init( const std::string              applicationName,
                     const std::string              engineName,
                     const std::vector<const char*> extensions,
                     const std::vector<const char*> validationLayers,
                     uint32_t                       applicationVersion,
                     uint32_t                       engineVersion,
                     uint32_t                       apiVersion )
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

    VK_CHECK_RESULT( vkCreateInstance( &instCreateInfo, nullptr, &this->id ) );
}

void Instance::deinit()
{
    if ( this->id != VK_NULL_HANDLE )
    {
        vkDestroyInstance( this->id, nullptr );
    }
}

std::vector<PhysicalDeviceInfo> Instance::getDeviceInfo()
{
    uint32_t deviceCount = 0;
    this->enumeratePhysicalDevices( &deviceCount, nullptr );

    std::vector<VkPhysicalDevice>   info( deviceCount );
    this->enumeratePhysicalDevices( &deviceCount, info.data() );

    std::vector<PhysicalDeviceInfo> out;
    out.reserve( info.size() );
    for ( auto& inf : info )
    {
        out.emplace_back( PhysicalDeviceInfo( *this, inf ) );
    }

    return out;
}

VkResult Instance::enumeratePhysicalDevices(
    uint32_t*         pPhysicalDeviceCount,
    VkPhysicalDevice* pPhysicalDevices
    )
{
    return vkEnumeratePhysicalDevices( this->id,
                                       pPhysicalDeviceCount,
                                       pPhysicalDevices );
}

VkResult Instance::enumerateInstanceLayerProperties(
    uint32_t*          pPropertyCount,
    VkLayerProperties* pProperties
    )
{
    return vkEnumerateInstanceLayerProperties( pPropertyCount,
                                               pProperties );
}

VkResult Instance::enumerateInstanceExtensionProperties(
    const char*            pLayerName,
    uint32_t*              pPropertyCount,
    VkExtensionProperties* pProperties
    )
{
    return vkEnumerateInstanceExtensionProperties( pLayerName,
                                                   pPropertyCount,
                                                   pProperties );
}

VkResult Instance::enumerateDeviceExtensionProperties(
    VkPhysicalDevice       physicalDevice,
    const char*            pLayerName,
    uint32_t*              pPropertyCount,
    VkExtensionProperties* pProperties
    )
{
    return vkEnumerateDeviceExtensionProperties( physicalDevice,
                                                 pLayerName,
                                                 pPropertyCount,
                                                 pProperties );
}

void Instance::getPhysicalDeviceFeatures(
    VkPhysicalDevice          physicalDevice,
    VkPhysicalDeviceFeatures* pFeatures
    )
{
    vkGetPhysicalDeviceFeatures( physicalDevice, pFeatures );
}

void Instance::getPhysicalDeviceProperties(
    VkPhysicalDevice            physicalDevice,
    VkPhysicalDeviceProperties* pProperties
    )
{
    vkGetPhysicalDeviceProperties( physicalDevice, pProperties );
}

void Instance::getPhysicalDeviceMemoryProperties(
    VkPhysicalDevice                  physicalDevice,
    VkPhysicalDeviceMemoryProperties* pMemoryProperties
    )
{
    vkGetPhysicalDeviceMemoryProperties( physicalDevice, pMemoryProperties );
}

void Instance::getPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice         physicalDevice,
    uint32_t*                pQueueFamilyPropertyCount,
    VkQueueFamilyProperties* pQueueFamilyProperties
    )
{
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        pQueueFamilyPropertyCount,
        pQueueFamilyProperties
        );
}

void Instance::getPhysicalDeviceFormatProperties(
    VkPhysicalDevice    physicalDevice,
    VkFormat            format,
    VkFormatProperties* pFormatProperties
    )
{
    vkGetPhysicalDeviceFormatProperties( physicalDevice,
                                         format,
                                         pFormatProperties );
}

VkResult Instance::getPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice         physicalDevice,
    VkFormat                 format,
    VkImageType              type,
    VkImageTiling            tiling,
    VkImageUsageFlags        usage,
    VkImageCreateFlags       flags,
    VkImageFormatProperties* pImageFormatProperties
    )
{
    return vkGetPhysicalDeviceImageFormatProperties(
        physicalDevice,
        format,
        type,
        tiling,
        usage,
        flags,
        pImageFormatProperties
        );
}

PhysicalDeviceFeatures::PhysicalDeviceFeatures(
    const PhysicalDeviceFeatures& features
    ) :
    robustBufferAccess( features.robustBufferAccess ),
    fullDrawIndexUint32( features.fullDrawIndexUint32 ),
    imageCubeArray( features.imageCubeArray ),
    independentBlend( features.independentBlend ),
    geometryShader( features.geometryShader ),
    tessellationShader( features.tessellationShader ),
    sampleRateShading( features.sampleRateShading ),
    dualSrcBlend( features.dualSrcBlend ),
    logicOp( features.logicOp ),
    multiDrawIndirect( features.multiDrawIndirect ),
    drawIndirectFirstInstance( features.drawIndirectFirstInstance ),
    depthClamp( features.depthClamp ),
    depthBiasClamp( features.depthBiasClamp ),
    fillModeNonSolid( features.fillModeNonSolid ),
    depthBounds( features.depthBounds ),
    wideLines( features.wideLines ),
    largePoints( features.largePoints ),
    alphaToOne( features.alphaToOne ),
    multiViewport( features.multiViewport ),
    samplerAnisotropy( features.samplerAnisotropy ),
    textureCompressionETC2( features.textureCompressionETC2 ),
    textureCompressionASTC_LDR( features.textureCompressionASTC_LDR ),
    textureCompressionBC( features.textureCompressionBC ),
    occlusionQueryPrecise( features.occlusionQueryPrecise ),
    pipelineStatisticsQuery( features.pipelineStatisticsQuery ),
    vertexPipelineStoresAndAtomics( features.vertexPipelineStoresAndAtomics ),
    fragmentStoresAndAtomics( features.fragmentStoresAndAtomics ),
    shaderTessellationAndGeometryPointSize( features.shaderTessellationAndGeometryPointSize ),
    shaderImageGatherExtended( features.shaderImageGatherExtended ),
    shaderStorageImageExtendedFormats( features.shaderStorageImageExtendedFormats ),
    shaderStorageImageMultisample( features.shaderStorageImageMultisample ),
    shaderStorageImageReadWithoutFormat( features.shaderStorageImageReadWithoutFormat ),
    shaderStorageImageWriteWithoutFormat( features.shaderStorageImageWriteWithoutFormat ),
    shaderUniformBufferArrayDynamicIndexing( features.shaderUniformBufferArrayDynamicIndexing ),
    shaderSampledImageArrayDynamicIndexing( features.shaderSampledImageArrayDynamicIndexing ),
    shaderStorageBufferArrayDynamicIndexing( features.shaderStorageBufferArrayDynamicIndexing ),
    shaderStorageImageArrayDynamicIndexing( features.shaderStorageImageArrayDynamicIndexing ),
    shaderClipDistance( features.shaderClipDistance ),
    shaderCullDistance( features.shaderCullDistance ),
    shaderFloat64( features.shaderFloat64 ),
    shaderInt64( features.shaderInt64 ),
    shaderInt16( features.shaderInt16 ),
    shaderResourceResidency( features.shaderResourceResidency ),
    shaderResourceMinLod( features.shaderResourceMinLod ),
    sparseBinding( features.sparseBinding ),
    sparseResidencyBuffer( features.sparseResidencyBuffer ),
    sparseResidencyImage2D( features.sparseResidencyImage2D ),
    sparseResidencyImage3D( features.sparseResidencyImage3D ),
    sparseResidency2Samples( features.sparseResidency2Samples ),
    sparseResidency4Samples( features.sparseResidency4Samples ),
    sparseResidency8Samples( features.sparseResidency8Samples ),
    sparseResidency16Samples( features.sparseResidency16Samples ),
    sparseResidencyAliased( features.sparseResidencyAliased ),
    variableMultisampleRate( features.variableMultisampleRate ),
    inheritedQueries( features.inheritedQueries )
{}

PhysicalDeviceFeatures::PhysicalDeviceFeatures(
    const VkPhysicalDeviceFeatures& features
    ) :
    robustBufferAccess( features.robustBufferAccess == VK_TRUE ? true : false ),
    fullDrawIndexUint32( features.fullDrawIndexUint32 == VK_TRUE ? true : false ),
    imageCubeArray( features.imageCubeArray == VK_TRUE ? true : false ),
    independentBlend( features.independentBlend == VK_TRUE ? true : false ),
    geometryShader( features.geometryShader == VK_TRUE ? true : false ),
    tessellationShader( features.tessellationShader == VK_TRUE ? true : false ),
    sampleRateShading( features.sampleRateShading == VK_TRUE ? true : false ),
    dualSrcBlend( features.dualSrcBlend == VK_TRUE ? true : false ),
    logicOp( features.logicOp == VK_TRUE ? true : false ),
    multiDrawIndirect( features.multiDrawIndirect == VK_TRUE ? true : false ),
    drawIndirectFirstInstance( features.drawIndirectFirstInstance == VK_TRUE ? true : false ),
    depthClamp( features.depthClamp == VK_TRUE ? true : false ),
    depthBiasClamp( features.depthBiasClamp == VK_TRUE ? true : false ),
    fillModeNonSolid( features.fillModeNonSolid == VK_TRUE ? true : false ),
    depthBounds( features.depthBounds == VK_TRUE ? true : false ),
    wideLines( features.wideLines == VK_TRUE ? true : false ),
    largePoints( features.largePoints == VK_TRUE ? true : false ),
    alphaToOne( features.alphaToOne == VK_TRUE ? true : false ),
    multiViewport( features.multiViewport == VK_TRUE ? true : false ),
    samplerAnisotropy( features.samplerAnisotropy == VK_TRUE ? true : false ),
    textureCompressionETC2( features.textureCompressionETC2 == VK_TRUE ? true : false ),
    textureCompressionASTC_LDR( features.textureCompressionASTC_LDR == VK_TRUE ? true : false ),
    textureCompressionBC( features.textureCompressionBC == VK_TRUE ? true : false ),
    occlusionQueryPrecise( features.occlusionQueryPrecise == VK_TRUE ? true : false ),
    pipelineStatisticsQuery( features.pipelineStatisticsQuery == VK_TRUE ? true : false ),
    vertexPipelineStoresAndAtomics( features.vertexPipelineStoresAndAtomics == VK_TRUE ? true : false ),
    fragmentStoresAndAtomics( features.fragmentStoresAndAtomics == VK_TRUE ? true : false ),
    shaderTessellationAndGeometryPointSize( features.shaderTessellationAndGeometryPointSize == VK_TRUE ? true : false ),
    shaderImageGatherExtended( features.shaderImageGatherExtended == VK_TRUE ? true : false ),
    shaderStorageImageExtendedFormats( features.shaderStorageImageExtendedFormats == VK_TRUE ? true : false ),
    shaderStorageImageMultisample( features.shaderStorageImageMultisample == VK_TRUE ? true : false ),
    shaderStorageImageReadWithoutFormat( features.shaderStorageImageReadWithoutFormat == VK_TRUE ? true : false ),
    shaderStorageImageWriteWithoutFormat( features.shaderStorageImageWriteWithoutFormat == VK_TRUE ? true : false ),
    shaderUniformBufferArrayDynamicIndexing( features.shaderUniformBufferArrayDynamicIndexing == VK_TRUE ? true : false ),
    shaderSampledImageArrayDynamicIndexing( features.shaderSampledImageArrayDynamicIndexing == VK_TRUE ? true : false ),
    shaderStorageBufferArrayDynamicIndexing( features.shaderStorageBufferArrayDynamicIndexing == VK_TRUE ? true : false ),
    shaderStorageImageArrayDynamicIndexing( features.shaderStorageImageArrayDynamicIndexing == VK_TRUE ? true : false ),
    shaderClipDistance( features.shaderClipDistance == VK_TRUE ? true : false ),
    shaderCullDistance( features.shaderCullDistance == VK_TRUE ? true : false ),
    shaderFloat64( features.shaderFloat64 == VK_TRUE ? true : false ),
    shaderInt64( features.shaderInt64 == VK_TRUE ? true : false ),
    shaderInt16( features.shaderInt16 == VK_TRUE ? true : false ),
    shaderResourceResidency( features.shaderResourceResidency == VK_TRUE ? true : false ),
    shaderResourceMinLod( features.shaderResourceMinLod == VK_TRUE ? true : false ),
    sparseBinding( features.sparseBinding == VK_TRUE ? true : false ),
    sparseResidencyBuffer( features.sparseResidencyBuffer == VK_TRUE ? true : false ),
    sparseResidencyImage2D( features.sparseResidencyImage2D == VK_TRUE ? true : false ),
    sparseResidencyImage3D( features.sparseResidencyImage3D == VK_TRUE ? true : false ),
    sparseResidency2Samples( features.sparseResidency2Samples == VK_TRUE ? true : false ),
    sparseResidency4Samples( features.sparseResidency4Samples == VK_TRUE ? true : false ),
    sparseResidency8Samples( features.sparseResidency8Samples == VK_TRUE ? true : false ),
    sparseResidency16Samples( features.sparseResidency16Samples == VK_TRUE ? true : false ),
    sparseResidencyAliased( features.sparseResidencyAliased == VK_TRUE ? true : false ),
    variableMultisampleRate( features.variableMultisampleRate == VK_TRUE ? true : false ),
    inheritedQueries( features.inheritedQueries == VK_TRUE ? true : false )
{}

PhysicalDeviceFeatures::PhysicalDeviceFeatures() :
    robustBufferAccess( false ),
    fullDrawIndexUint32( false ),
    imageCubeArray( false ),
    independentBlend( false ),
    geometryShader( false ),
    tessellationShader( false ),
    sampleRateShading( false ),
    dualSrcBlend( false ),
    logicOp( false ),
    multiDrawIndirect( false ),
    drawIndirectFirstInstance( false ),
    depthClamp( false ),
    depthBiasClamp( false ),
    fillModeNonSolid( false ),
    depthBounds( false ),
    wideLines( false ),
    largePoints( false ),
    alphaToOne( false ),
    multiViewport( false ),
    samplerAnisotropy( false ),
    textureCompressionETC2( false ),
    textureCompressionASTC_LDR( false ),
    textureCompressionBC( false ),
    occlusionQueryPrecise( false ),
    pipelineStatisticsQuery( false ),
    vertexPipelineStoresAndAtomics( false ),
    fragmentStoresAndAtomics( false ),
    shaderTessellationAndGeometryPointSize( false ),
    shaderImageGatherExtended( false ),
    shaderStorageImageExtendedFormats( false ),
    shaderStorageImageMultisample( false ),
    shaderStorageImageReadWithoutFormat( false ),
    shaderStorageImageWriteWithoutFormat( false ),
    shaderUniformBufferArrayDynamicIndexing( false ),
    shaderSampledImageArrayDynamicIndexing( false ),
    shaderStorageBufferArrayDynamicIndexing( false ),
    shaderStorageImageArrayDynamicIndexing( false ),
    shaderClipDistance( false ),
    shaderCullDistance( false ),
    shaderFloat64( false ),
    shaderInt64( false ),
    shaderInt16( false ),
    shaderResourceResidency( false ),
    shaderResourceMinLod( false ),
    sparseBinding( false ),
    sparseResidencyBuffer( false ),
    sparseResidencyImage2D( false ),
    sparseResidencyImage3D( false ),
    sparseResidency2Samples( false ),
    sparseResidency4Samples( false ),
    sparseResidency8Samples( false ),
    sparseResidency16Samples( false ),
    sparseResidencyAliased( false ),
    variableMultisampleRate( false ),
    inheritedQueries( false )
{}

VkPhysicalDeviceFeatures PhysicalDeviceFeatures::makeEnabledList()
{
    VkPhysicalDeviceFeatures features;
    features.robustBufferAccess                      = this->robustBufferAccess ? VK_TRUE : VK_FALSE;
    features.fullDrawIndexUint32                     = this->fullDrawIndexUint32 ? VK_TRUE : VK_FALSE;
    features.imageCubeArray                          = this->imageCubeArray ? VK_TRUE : VK_FALSE;
    features.independentBlend                        = this->independentBlend ? VK_TRUE : VK_FALSE;
    features.geometryShader                          = this->geometryShader ? VK_TRUE : VK_FALSE;
    features.tessellationShader                      = this->tessellationShader ? VK_TRUE : VK_FALSE;
    features.sampleRateShading                       = this->sampleRateShading ? VK_TRUE : VK_FALSE;
    features.dualSrcBlend                            = this->dualSrcBlend ? VK_TRUE : VK_FALSE;
    features.logicOp                                 = this->logicOp ? VK_TRUE : VK_FALSE;
    features.multiDrawIndirect                       = this->multiDrawIndirect ? VK_TRUE : VK_FALSE;
    features.drawIndirectFirstInstance               = this->drawIndirectFirstInstance ? VK_TRUE : VK_FALSE;
    features.depthClamp                              = this->depthClamp ? VK_TRUE : VK_FALSE;
    features.depthBiasClamp                          = this->depthBiasClamp ? VK_TRUE : VK_FALSE;
    features.fillModeNonSolid                        = this->fillModeNonSolid ? VK_TRUE : VK_FALSE;
    features.depthBounds                             = this->depthBounds ? VK_TRUE : VK_FALSE;
    features.wideLines                               = this->wideLines ? VK_TRUE : VK_FALSE;
    features.largePoints                             = this->largePoints ? VK_TRUE : VK_FALSE;
    features.alphaToOne                              = this->alphaToOne ? VK_TRUE : VK_FALSE;
    features.multiViewport                           = this->multiViewport ? VK_TRUE : VK_FALSE;
    features.samplerAnisotropy                       = this->samplerAnisotropy ? VK_TRUE : VK_FALSE;
    features.textureCompressionETC2                  = this->textureCompressionETC2 ? VK_TRUE : VK_FALSE;
    features.textureCompressionASTC_LDR              = this->textureCompressionASTC_LDR ? VK_TRUE : VK_FALSE;
    features.textureCompressionBC                    = this->textureCompressionBC ? VK_TRUE : VK_FALSE;
    features.occlusionQueryPrecise                   = this->occlusionQueryPrecise ? VK_TRUE : VK_FALSE;
    features.pipelineStatisticsQuery                 = this->pipelineStatisticsQuery ? VK_TRUE : VK_FALSE;
    features.vertexPipelineStoresAndAtomics          = this->vertexPipelineStoresAndAtomics ? VK_TRUE : VK_FALSE;
    features.fragmentStoresAndAtomics                = this->fragmentStoresAndAtomics ? VK_TRUE : VK_FALSE;
    features.shaderTessellationAndGeometryPointSize  = this->shaderTessellationAndGeometryPointSize ? VK_TRUE : VK_FALSE;
    features.shaderImageGatherExtended               = this->shaderImageGatherExtended ? VK_TRUE : VK_FALSE;
    features.shaderStorageImageExtendedFormats       = this->shaderStorageImageExtendedFormats ? VK_TRUE : VK_FALSE;
    features.shaderStorageImageMultisample           = this->shaderStorageImageMultisample ? VK_TRUE : VK_FALSE;
    features.shaderStorageImageReadWithoutFormat     = this->shaderStorageImageReadWithoutFormat ? VK_TRUE : VK_FALSE;
    features.shaderStorageImageWriteWithoutFormat    = this->shaderStorageImageWriteWithoutFormat ? VK_TRUE : VK_FALSE;
    features.shaderUniformBufferArrayDynamicIndexing = this->shaderUniformBufferArrayDynamicIndexing ? VK_TRUE : VK_FALSE;
    features.shaderSampledImageArrayDynamicIndexing  = this->shaderSampledImageArrayDynamicIndexing ? VK_TRUE : VK_FALSE;
    features.shaderStorageBufferArrayDynamicIndexing = this->shaderStorageBufferArrayDynamicIndexing ? VK_TRUE : VK_FALSE;
    features.shaderStorageImageArrayDynamicIndexing  = this->shaderStorageImageArrayDynamicIndexing ? VK_TRUE : VK_FALSE;
    features.shaderClipDistance                      = this->shaderClipDistance ? VK_TRUE : VK_FALSE;
    features.shaderCullDistance                      = this->shaderCullDistance ? VK_TRUE : VK_FALSE;
    features.shaderFloat64                           = this->shaderFloat64 ? VK_TRUE : VK_FALSE;
    features.shaderInt64                             = this->shaderInt64 ? VK_TRUE : VK_FALSE;
    features.shaderInt16                             = this->shaderInt16 ? VK_TRUE : VK_FALSE;
    features.shaderResourceResidency                 = this->shaderResourceResidency ? VK_TRUE : VK_FALSE;
    features.shaderResourceMinLod                    = this->shaderResourceMinLod ? VK_TRUE : VK_FALSE;
    features.sparseBinding                           = this->sparseBinding ? VK_TRUE : VK_FALSE;
    features.sparseResidencyBuffer                   = this->sparseResidencyBuffer ? VK_TRUE : VK_FALSE;
    features.sparseResidencyImage2D                  = this->sparseResidencyImage2D ? VK_TRUE : VK_FALSE;
    features.sparseResidencyImage3D                  = this->sparseResidencyImage3D ? VK_TRUE : VK_FALSE;
    features.sparseResidency2Samples                 = this->sparseResidency2Samples ? VK_TRUE : VK_FALSE;
    features.sparseResidency4Samples                 = this->sparseResidency4Samples ? VK_TRUE : VK_FALSE;
    features.sparseResidency8Samples                 = this->sparseResidency8Samples ? VK_TRUE : VK_FALSE;
    features.sparseResidency16Samples                = this->sparseResidency16Samples ? VK_TRUE : VK_FALSE;
    features.sparseResidencyAliased                  = this->sparseResidencyAliased ? VK_TRUE : VK_FALSE;
    features.variableMultisampleRate                 = this->variableMultisampleRate ? VK_TRUE : VK_FALSE;
    features.inheritedQueries                        = this->inheritedQueries ? VK_TRUE : VK_FALSE;
 
    return features;
}

PhysicalDeviceLimits::PhysicalDeviceLimits( const PhysicalDeviceLimits& limits ) :
    maxImageDimension1D( limits.maxImageDimension1D ),
    maxImageDimension2D( limits.maxImageDimension2D ),
    maxImageDimension3D( limits.maxImageDimension3D ),
    maxImageDimensionCube( limits.maxImageDimensionCube ),
    maxImageArrayLayers( limits.maxImageArrayLayers ),
    maxTexelBufferElements( limits.maxTexelBufferElements ),
    maxUniformBufferRange( limits.maxUniformBufferRange ),
    maxStorageBufferRange( limits.maxStorageBufferRange ),
    maxPushConstantsSize( limits.maxPushConstantsSize ),
    maxMemoryAllocationCount( limits.maxMemoryAllocationCount ),
    maxSamplerAllocationCount( limits.maxSamplerAllocationCount ),
    bufferImageGranularity( limits.bufferImageGranularity ),
    sparseAddressSpaceSize( limits.sparseAddressSpaceSize ),
    maxBoundDescriptorSets( limits.maxBoundDescriptorSets ),
    maxPerStageDescriptorSamplers( limits.maxPerStageDescriptorSamplers ),
    maxPerStageDescriptorUniformBuffers( limits.maxPerStageDescriptorUniformBuffers ),
    maxPerStageDescriptorStorageBuffers( limits.maxPerStageDescriptorStorageBuffers ),
    maxPerStageDescriptorSampledImages( limits.maxPerStageDescriptorSampledImages ),
    maxPerStageDescriptorStorageImages( limits.maxPerStageDescriptorStorageImages ),
    maxPerStageDescriptorInputAttachments( limits.maxPerStageDescriptorInputAttachments ),
    maxPerStageResources( limits.maxPerStageResources ),
    maxDescriptorSetSamplers( limits.maxDescriptorSetSamplers ),
    maxDescriptorSetUniformBuffers( limits.maxDescriptorSetUniformBuffers ),
    maxDescriptorSetUniformBuffersDynamic( limits.maxDescriptorSetUniformBuffersDynamic ),
    maxDescriptorSetStorageBuffers( limits.maxDescriptorSetStorageBuffers ),
    maxDescriptorSetStorageBuffersDynamic( limits.maxDescriptorSetStorageBuffersDynamic ),
    maxDescriptorSetSampledImages( limits.maxDescriptorSetSampledImages ),
    maxDescriptorSetStorageImages( limits.maxDescriptorSetStorageImages ),
    maxDescriptorSetInputAttachments( limits.maxDescriptorSetInputAttachments ),
    maxVertexInputAttributes( limits.maxVertexInputAttributes ),
    maxVertexInputBindings( limits.maxVertexInputBindings ),
    maxVertexInputAttributeOffset( limits.maxVertexInputAttributeOffset ),
    maxVertexInputBindingStride( limits.maxVertexInputBindingStride ),
    maxVertexOutputComponents( limits.maxVertexOutputComponents ),
    maxTessellationGenerationLevel( limits.maxTessellationGenerationLevel ),
    maxTessellationPatchSize( limits.maxTessellationPatchSize ),
    maxTessellationControlPerVertexInputComponents( limits.maxTessellationControlPerVertexInputComponents ),
    maxTessellationControlPerVertexOutputComponents( limits.maxTessellationControlPerVertexOutputComponents ),
    maxTessellationControlPerPatchOutputComponents( limits.maxTessellationControlPerPatchOutputComponents ),
    maxTessellationControlTotalOutputComponents( limits.maxTessellationControlTotalOutputComponents ),
    maxTessellationEvaluationInputComponents( limits.maxTessellationEvaluationInputComponents ),
    maxTessellationEvaluationOutputComponents( limits.maxTessellationEvaluationOutputComponents ),
    maxGeometryShaderInvocations( limits.maxGeometryShaderInvocations ),
    maxGeometryInputComponents( limits.maxGeometryInputComponents ),
    maxGeometryOutputComponents( limits.maxGeometryOutputComponents ),
    maxGeometryOutputVertices( limits.maxGeometryOutputVertices ),
    maxGeometryTotalOutputComponents( limits.maxGeometryTotalOutputComponents ),
    maxFragmentInputComponents( limits.maxFragmentInputComponents ),
    maxFragmentOutputAttachments( limits.maxFragmentOutputAttachments ),
    maxFragmentDualSrcAttachments( limits.maxFragmentDualSrcAttachments ),
    maxFragmentCombinedOutputResources( limits.maxFragmentCombinedOutputResources ),
    maxComputeSharedMemorySize( limits.maxComputeSharedMemorySize ),
    maxComputeWorkGroupInvocations( limits.maxComputeWorkGroupInvocations ),
    subPixelPrecisionBits( limits.subPixelPrecisionBits ),
    subTexelPrecisionBits( limits.subTexelPrecisionBits ),
    mipmapPrecisionBits( limits.mipmapPrecisionBits ),
    maxDrawIndexedIndexValue( limits.maxDrawIndexedIndexValue ),
    maxDrawIndirectCount( limits.maxDrawIndirectCount ),
    maxSamplerLodBias( limits.maxSamplerLodBias ),
    maxSamplerAnisotropy( limits.maxSamplerAnisotropy ),
    maxViewports( limits.maxViewports ),
    viewportSubPixelBits( limits.viewportSubPixelBits ),
    minMemoryMapAlignment( limits.minMemoryMapAlignment ),
    minTexelBufferOffsetAlignment( limits.minTexelBufferOffsetAlignment ),
    minUniformBufferOffsetAlignment( limits.minUniformBufferOffsetAlignment ),
    minStorageBufferOffsetAlignment( limits.minStorageBufferOffsetAlignment ),
    minTexelOffset( limits.minTexelOffset ),
    maxTexelOffset( limits.maxTexelOffset ),
    minTexelGatherOffset( limits.minTexelGatherOffset ),
    maxTexelGatherOffset( limits.maxTexelGatherOffset ),
    minInterpolationOffset( limits.minInterpolationOffset ),
    maxInterpolationOffset( limits.maxInterpolationOffset ),
    subPixelInterpolationOffsetBits( limits.subPixelInterpolationOffsetBits ),
    maxFramebufferWidth( limits.maxFramebufferWidth ),
    maxFramebufferHeight( limits.maxFramebufferHeight ),
    maxFramebufferLayers( limits.maxFramebufferLayers ),
    framebufferColorSampleCounts( limits.framebufferColorSampleCounts ),
    framebufferDepthSampleCounts( limits.framebufferDepthSampleCounts ),
    framebufferStencilSampleCounts( limits.framebufferStencilSampleCounts ),
    framebufferNoAttachmentsSampleCounts( limits.framebufferNoAttachmentsSampleCounts ),
    maxColorAttachments( limits.maxColorAttachments ),
    sampledImageColorSampleCounts( limits.sampledImageColorSampleCounts ),
    sampledImageIntegerSampleCounts( limits.sampledImageIntegerSampleCounts ),
    sampledImageDepthSampleCounts( limits.sampledImageDepthSampleCounts ),
    sampledImageStencilSampleCounts( limits.sampledImageStencilSampleCounts ),
    storageImageSampleCounts( limits.storageImageSampleCounts ),
    maxSampleMaskWords( limits.maxSampleMaskWords ),
    timestampComputeAndGraphics( limits.timestampComputeAndGraphics ),
    timestampPeriod( limits.timestampPeriod ),
    maxClipDistances( limits.maxClipDistances ),
    maxCullDistances( limits.maxCullDistances ),
    maxCombinedClipAndCullDistances( limits.maxCombinedClipAndCullDistances ),
    discreteQueuePriorities( limits.discreteQueuePriorities ),
    pointSizeGranularity( limits.pointSizeGranularity ),
    lineWidthGranularity( limits.lineWidthGranularity ),
    strictLines( limits.strictLines ),
    standardSampleLocations( limits.standardSampleLocations ),
    optimalBufferCopyOffsetAlignment( limits.optimalBufferCopyOffsetAlignment ),
    optimalBufferCopyRowPitchAlignment( limits.optimalBufferCopyRowPitchAlignment ),
    nonCoherentAtomSize( limits.nonCoherentAtomSize )
{
    std::copy( std::begin( limits.maxComputeWorkGroupCount ),
               std::end( limits.maxComputeWorkGroupCount ),
               std::begin( this->maxComputeWorkGroupCount ) );

    std::copy( std::begin( limits.maxComputeWorkGroupSize ),
               std::end( limits.maxComputeWorkGroupSize ),
               std::begin( this->maxComputeWorkGroupSize ) );

    std::copy( std::begin( limits.maxViewportDimensions ),
               std::end( limits.maxViewportDimensions ),
               std::begin( this->maxViewportDimensions ) );

    std::copy( std::begin( limits.viewportBoundsRange ),
               std::end( limits.viewportBoundsRange ),
               std::begin( this->viewportBoundsRange ) );

    std::copy( std::begin( limits.pointSizeRange ),
               std::end( limits.pointSizeRange ),
               std::begin( this->pointSizeRange ) );

    std::copy( std::begin( limits.lineWidthRange ),
               std::end( limits.lineWidthRange ),
               std::begin( this->lineWidthRange ) );
}

PhysicalDeviceLimits::PhysicalDeviceLimits( const VkPhysicalDeviceLimits& limits ) :
    maxImageDimension1D( limits.maxImageDimension1D ),
    maxImageDimension2D( limits.maxImageDimension2D ),
    maxImageDimension3D( limits.maxImageDimension3D ),
    maxImageDimensionCube( limits.maxImageDimensionCube ),
    maxImageArrayLayers( limits.maxImageArrayLayers ),
    maxTexelBufferElements( limits.maxTexelBufferElements ),
    maxUniformBufferRange( limits.maxUniformBufferRange ),
    maxStorageBufferRange( limits.maxStorageBufferRange ),
    maxPushConstantsSize( limits.maxPushConstantsSize ),
    maxMemoryAllocationCount( limits.maxMemoryAllocationCount ),
    maxSamplerAllocationCount( limits.maxSamplerAllocationCount ),
    bufferImageGranularity( limits.bufferImageGranularity ),
    sparseAddressSpaceSize( limits.sparseAddressSpaceSize ),
    maxBoundDescriptorSets( limits.maxBoundDescriptorSets ),
    maxPerStageDescriptorSamplers( limits.maxPerStageDescriptorSamplers ),
    maxPerStageDescriptorUniformBuffers( limits.maxPerStageDescriptorUniformBuffers ),
    maxPerStageDescriptorStorageBuffers( limits.maxPerStageDescriptorStorageBuffers ),
    maxPerStageDescriptorSampledImages( limits.maxPerStageDescriptorSampledImages ),
    maxPerStageDescriptorStorageImages( limits.maxPerStageDescriptorStorageImages ),
    maxPerStageDescriptorInputAttachments( limits.maxPerStageDescriptorInputAttachments ),
    maxPerStageResources( limits.maxPerStageResources ),
    maxDescriptorSetSamplers( limits.maxDescriptorSetSamplers ),
    maxDescriptorSetUniformBuffers( limits.maxDescriptorSetUniformBuffers ),
    maxDescriptorSetUniformBuffersDynamic( limits.maxDescriptorSetUniformBuffersDynamic ),
    maxDescriptorSetStorageBuffers( limits.maxDescriptorSetStorageBuffers ),
    maxDescriptorSetStorageBuffersDynamic( limits.maxDescriptorSetStorageBuffersDynamic ),
    maxDescriptorSetSampledImages( limits.maxDescriptorSetSampledImages ),
    maxDescriptorSetStorageImages( limits.maxDescriptorSetStorageImages ),
    maxDescriptorSetInputAttachments( limits.maxDescriptorSetInputAttachments ),
    maxVertexInputAttributes( limits.maxVertexInputAttributes ),
    maxVertexInputBindings( limits.maxVertexInputBindings ),
    maxVertexInputAttributeOffset( limits.maxVertexInputAttributeOffset ),
    maxVertexInputBindingStride( limits.maxVertexInputBindingStride ),
    maxVertexOutputComponents( limits.maxVertexOutputComponents ),
    maxTessellationGenerationLevel( limits.maxTessellationGenerationLevel ),
    maxTessellationPatchSize( limits.maxTessellationPatchSize ),
    maxTessellationControlPerVertexInputComponents( limits.maxTessellationControlPerVertexInputComponents ),
    maxTessellationControlPerVertexOutputComponents( limits.maxTessellationControlPerVertexOutputComponents ),
    maxTessellationControlPerPatchOutputComponents( limits.maxTessellationControlPerPatchOutputComponents ),
    maxTessellationControlTotalOutputComponents( limits.maxTessellationControlTotalOutputComponents ),
    maxTessellationEvaluationInputComponents( limits.maxTessellationEvaluationInputComponents ),
    maxTessellationEvaluationOutputComponents( limits.maxTessellationEvaluationOutputComponents ),
    maxGeometryShaderInvocations( limits.maxGeometryShaderInvocations ),
    maxGeometryInputComponents( limits.maxGeometryInputComponents ),
    maxGeometryOutputComponents( limits.maxGeometryOutputComponents ),
    maxGeometryOutputVertices( limits.maxGeometryOutputVertices ),
    maxGeometryTotalOutputComponents( limits.maxGeometryTotalOutputComponents ),
    maxFragmentInputComponents( limits.maxFragmentInputComponents ),
    maxFragmentOutputAttachments( limits.maxFragmentOutputAttachments ),
    maxFragmentDualSrcAttachments( limits.maxFragmentDualSrcAttachments ),
    maxFragmentCombinedOutputResources( limits.maxFragmentCombinedOutputResources ),
    maxComputeSharedMemorySize( limits.maxComputeSharedMemorySize ),
    maxComputeWorkGroupInvocations( limits.maxComputeWorkGroupInvocations ),
    subPixelPrecisionBits( limits.subPixelPrecisionBits ),
    subTexelPrecisionBits( limits.subTexelPrecisionBits ),
    mipmapPrecisionBits( limits.mipmapPrecisionBits ),
    maxDrawIndexedIndexValue( limits.maxDrawIndexedIndexValue ),
    maxDrawIndirectCount( limits.maxDrawIndirectCount ),
    maxSamplerLodBias( limits.maxSamplerLodBias ),
    maxSamplerAnisotropy( limits.maxSamplerAnisotropy ),
    maxViewports( limits.maxViewports ),
    viewportSubPixelBits( limits.viewportSubPixelBits ),
    minMemoryMapAlignment( limits.minMemoryMapAlignment ),
    minTexelBufferOffsetAlignment( limits.minTexelBufferOffsetAlignment ),
    minUniformBufferOffsetAlignment( limits.minUniformBufferOffsetAlignment ),
    minStorageBufferOffsetAlignment( limits.minStorageBufferOffsetAlignment ),
    minTexelOffset( limits.minTexelOffset ),
    maxTexelOffset( limits.maxTexelOffset ),
    minTexelGatherOffset( limits.minTexelGatherOffset ),
    maxTexelGatherOffset( limits.maxTexelGatherOffset ),
    minInterpolationOffset( limits.minInterpolationOffset ),
    maxInterpolationOffset( limits.maxInterpolationOffset ),
    subPixelInterpolationOffsetBits( limits.subPixelInterpolationOffsetBits ),
    maxFramebufferWidth( limits.maxFramebufferWidth ),
    maxFramebufferHeight( limits.maxFramebufferHeight ),
    maxFramebufferLayers( limits.maxFramebufferLayers ),
    framebufferColorSampleCounts( limits.framebufferColorSampleCounts ),
    framebufferDepthSampleCounts( limits.framebufferDepthSampleCounts ),
    framebufferStencilSampleCounts( limits.framebufferStencilSampleCounts ),
    framebufferNoAttachmentsSampleCounts( limits.framebufferNoAttachmentsSampleCounts ),
    maxColorAttachments( limits.maxColorAttachments ),
    sampledImageColorSampleCounts( limits.sampledImageColorSampleCounts ),
    sampledImageIntegerSampleCounts( limits.sampledImageIntegerSampleCounts ),
    sampledImageDepthSampleCounts( limits.sampledImageDepthSampleCounts ),
    sampledImageStencilSampleCounts( limits.sampledImageStencilSampleCounts ),
    storageImageSampleCounts( limits.storageImageSampleCounts ),
    maxSampleMaskWords( limits.maxSampleMaskWords ),
    timestampComputeAndGraphics( limits.timestampComputeAndGraphics ),
    timestampPeriod( limits.timestampPeriod ),
    maxClipDistances( limits.maxClipDistances ),
    maxCullDistances( limits.maxCullDistances ),
    maxCombinedClipAndCullDistances( limits.maxCombinedClipAndCullDistances ),
    discreteQueuePriorities( limits.discreteQueuePriorities ),
    pointSizeGranularity( limits.pointSizeGranularity ),
    lineWidthGranularity( limits.lineWidthGranularity ),
    strictLines( limits.strictLines ),
    standardSampleLocations( limits.standardSampleLocations ),
    optimalBufferCopyOffsetAlignment( limits.optimalBufferCopyOffsetAlignment ),
    optimalBufferCopyRowPitchAlignment( limits.optimalBufferCopyRowPitchAlignment ),
    nonCoherentAtomSize( limits.nonCoherentAtomSize )
{
    std::copy( std::begin( limits.maxComputeWorkGroupCount ),
               std::end( limits.maxComputeWorkGroupCount ),
               std::begin( this->maxComputeWorkGroupCount ) );

    std::copy( std::begin( limits.maxComputeWorkGroupSize ),
               std::end( limits.maxComputeWorkGroupSize ),
               std::begin( this->maxComputeWorkGroupSize ) );

    std::copy( std::begin( limits.maxViewportDimensions ),
               std::end( limits.maxViewportDimensions ),
               std::begin( this->maxViewportDimensions ) );

    std::copy( std::begin( limits.viewportBoundsRange ),
               std::end( limits.viewportBoundsRange ),
               std::begin( this->viewportBoundsRange ) );

    std::copy( std::begin( limits.pointSizeRange ),
               std::end( limits.pointSizeRange ),
               std::begin( this->pointSizeRange ) );

    std::copy( std::begin( limits.lineWidthRange ),
               std::end( limits.lineWidthRange ),
               std::begin( this->lineWidthRange ) );
}

PhysicalDeviceQueueFamilyProperties::PhysicalDeviceQueueFamilyProperties(
    const PhysicalDeviceQueueFamilyProperties& props
    ) :
    graphics( props.graphics ),
    compute( props.compute ),
    transfer( props.transfer ),
    sparse( props.sparse ),
    count( props.count ),
    timestampValidBits( props.timestampValidBits ),
    minImageTransferGranularity( props.minImageTransferGranularity )
{}

PhysicalDeviceQueueFamilyProperties::PhysicalDeviceQueueFamilyProperties(
    const VkQueueFamilyProperties& props
    ) :
    graphics( ( props.queueFlags & VK_QUEUE_GRAPHICS_BIT ) > 0 ? true : false ),
    compute( ( props.queueFlags & VK_QUEUE_COMPUTE_BIT ) > 0 ? true : false ),
    transfer( ( props.queueFlags & VK_QUEUE_TRANSFER_BIT ) > 0 ? true : false ),
    sparse( ( props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ) > 0 ? true : false ),
    count( props.queueCount ),
    timestampValidBits( props.timestampValidBits ),
    minImageTransferGranularity( props.minImageTransferGranularity )

{}

PhysicalDeviceInfo::PhysicalDeviceInfo( Instance&        instance,
                                        VkPhysicalDevice physical )
{
    // Get basic device info.
    VkPhysicalDeviceProperties props;
    instance.getPhysicalDeviceProperties( physical, &props );
    
    this->vendorID = props.vendorID;
    this->deviceID = props.deviceID;

    switch( props.deviceType )
    {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        this->type = PhysicalDeviceType::DISCRETE;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        this->type = PhysicalDeviceType::INTEGRATED;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        this->type = PhysicalDeviceType::VIRTUAL;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        this->type = PhysicalDeviceType::CPU;
        break;
    default:
        this->type = PhysicalDeviceType::OTHER;
        break;
    }

    // Get Device Limits.
    this->limits = PhysicalDeviceLimits( props.limits );

    // Get Device Feature Support.
    VkPhysicalDeviceFeatures feats;
    instance.getPhysicalDeviceFeatures( physical, &feats );
    this->features = PhysicalDeviceFeatures( feats );

    // Get Queue Info.
    uint32_t                             queueFamilyCount;
    instance.getPhysicalDeviceQueueFamilyProperties( physical,
                                                     &queueFamilyCount,
                                                     nullptr );
    std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
    this->queueFamilyInfo.reserve( queueFamilyCount );
    
    instance.getPhysicalDeviceQueueFamilyProperties( physical,
                                                     &queueFamilyCount,
                                                     queueFamilies.data() );

    for ( auto& family : queueFamilies )
    {
        this->queueFamilyInfo.emplace_back( PhysicalDeviceQueueFamilyProperties( family ) );
    }
}
