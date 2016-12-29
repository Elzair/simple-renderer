#pragma once

#include <array>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

struct PhysicalDeviceInfo;
struct PhysicalDeviceFeatures;
struct PhysicalDeviceLimits;
struct PhysicalDeviceQueueFamilyProperties;

class Instance
{
    friend class PhysicalDeviceInfo;
    
public:
    
    VkInstance id = VK_NULL_HANDLE;

    Instance(
        const std::string              applicationName,
        const std::string              engineName,
        const std::vector<const char*> extensions,
        const std::vector<const char*> validationLayers,
        uint32_t                       applicationVersion = 1,
        uint32_t                       engineVersion      = 1,
        uint32_t                       apiVersion         = VK_API_VERSION_1_0
        )
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

    std::vector<PhysicalDeviceInfo> getDeviceInfo();

private:

    VkResult enumeratePhysicalDevices( uint32_t*         pPhysicalDeviceCount,
                                       VkPhysicalDevice* pPhysicalDevices );

    VkResult enumerateInstanceLayerProperties(
        uint32_t*          pPropertyCount,
        VkLayerProperties* pProperties
        );

    VkResult enumerateInstanceExtensionProperties(
        const char*            pLayerName,
        uint32_t*              pPropertyCount,
        VkExtensionProperties* pProperties
        );

    VkResult enumerateDeviceExtensionProperties(
        VkPhysicalDevice       physicalDevice,
        const char*            pLayerName,
        uint32_t*              pPropertyCount,
        VkExtensionProperties* pProperties
        );

    void getPhysicalDeviceFeatures( VkPhysicalDevice          physicalDevice,
                                    VkPhysicalDeviceFeatures* pFeatures );

    void getPhysicalDeviceProperties( VkPhysicalDevice            physicalDevice,
                                      VkPhysicalDeviceProperties* pProperties );

    void getPhysicalDeviceMemoryProperties(
        VkPhysicalDevice                  physicalDevice,
        VkPhysicalDeviceMemoryProperties* pMemoryProperties
        );

    void getPhysicalDeviceQueueFamilyProperties(
        VkPhysicalDevice         physicalDevice,
        uint32_t*                pQueueFamilyPropertyCount,
        VkQueueFamilyProperties* pQueueFamilyProperties
        );

    void getPhysicalDeviceFormatProperties(
        VkPhysicalDevice    physicalDevice,
        VkFormat            format,
        VkFormatProperties* pFormatProperties
        );

    VkResult getPhysicalDeviceImageFormatProperties(
        VkPhysicalDevice         physicalDevice,
        VkFormat                 format,
        VkImageType              type,
        VkImageTiling            tiling,
        VkImageUsageFlags        usage,
        VkImageCreateFlags       flags,
        VkImageFormatProperties* pImageFormatProperties
        );
};

struct PhysicalDeviceFeatures
{
    friend class PhysicalDeviceInfo;
    
public:
    
    bool robustBufferAccess;
    bool fullDrawIndexUint32;
    bool imageCubeArray;
    bool independentBlend;
    bool geometryShader;
    bool tessellationShader;
    bool sampleRateShading;
    bool dualSrcBlend;
    bool logicOp;
    bool multiDrawIndirect;
    bool drawIndirectFirstInstance;
    bool depthClamp;
    bool depthBiasClamp;
    bool fillModeNonSolid;
    bool depthBounds;
    bool wideLines;
    bool largePoints;
    bool alphaToOne;
    bool multiViewport;
    bool samplerAnisotropy;
    bool textureCompressionETC2;
    bool textureCompressionASTC_LDR;
    bool textureCompressionBC;
    bool occlusionQueryPrecise;
    bool pipelineStatisticsQuery;
    bool vertexPipelineStoresAndAtomics;
    bool fragmentStoresAndAtomics;
    bool shaderTessellationAndGeometryPointSize;
    bool shaderImageGatherExtended;
    bool shaderStorageImageExtendedFormats;
    bool shaderStorageImageMultisample;
    bool shaderStorageImageReadWithoutFormat;
    bool shaderStorageImageWriteWithoutFormat;
    bool shaderUniformBufferArrayDynamicIndexing;
    bool shaderSampledImageArrayDynamicIndexing;
    bool shaderStorageBufferArrayDynamicIndexing;
    bool shaderStorageImageArrayDynamicIndexing;
    bool shaderClipDistance;
    bool shaderCullDistance;
    bool shaderFloat64;
    bool shaderInt64;
    bool shaderInt16;
    bool shaderResourceResidency;
    bool shaderResourceMinLod;
    bool sparseBinding;
    bool sparseResidencyBuffer;
    bool sparseResidencyImage2D;
    bool sparseResidencyImage3D;
    bool sparseResidency2Samples;
    bool sparseResidency4Samples;
    bool sparseResidency8Samples;
    bool sparseResidency16Samples;
    bool sparseResidencyAliased;
    bool variableMultisampleRate;
    bool inheritedQueries;

    PhysicalDeviceFeatures();
    PhysicalDeviceFeatures( const PhysicalDeviceFeatures& features );

private:

    PhysicalDeviceFeatures( const VkPhysicalDeviceFeatures& features );

    VkPhysicalDeviceFeatures makeEnabledList();
};

struct PhysicalDeviceLimits
{
    friend class PhysicalDeviceInfo;
    
public:
    
    uint32_t               maxImageDimension1D;
    uint32_t               maxImageDimension2D;
    uint32_t               maxImageDimension3D;
    uint32_t               maxImageDimensionCube;
    uint32_t               maxImageArrayLayers;
    uint32_t               maxTexelBufferElements;
    uint32_t               maxUniformBufferRange;
    uint32_t               maxStorageBufferRange;
    uint32_t               maxPushConstantsSize;
    uint32_t               maxMemoryAllocationCount;
    uint32_t               maxSamplerAllocationCount;
    uint64_t               bufferImageGranularity;
    uint64_t               sparseAddressSpaceSize;
    uint32_t               maxBoundDescriptorSets;
    uint32_t               maxPerStageDescriptorSamplers;
    uint32_t               maxPerStageDescriptorUniformBuffers;
    uint32_t               maxPerStageDescriptorStorageBuffers;
    uint32_t               maxPerStageDescriptorSampledImages;
    uint32_t               maxPerStageDescriptorStorageImages;
    uint32_t               maxPerStageDescriptorInputAttachments;
    uint32_t               maxPerStageResources;
    uint32_t               maxDescriptorSetSamplers;
    uint32_t               maxDescriptorSetUniformBuffers;
    uint32_t               maxDescriptorSetUniformBuffersDynamic;
    uint32_t               maxDescriptorSetStorageBuffers;
    uint32_t               maxDescriptorSetStorageBuffersDynamic;
    uint32_t               maxDescriptorSetSampledImages;
    uint32_t               maxDescriptorSetStorageImages;
    uint32_t               maxDescriptorSetInputAttachments;
    uint32_t               maxVertexInputAttributes;
    uint32_t               maxVertexInputBindings;
    uint32_t               maxVertexInputAttributeOffset;
    uint32_t               maxVertexInputBindingStride;
    uint32_t               maxVertexOutputComponents;
    uint32_t               maxTessellationGenerationLevel;
    uint32_t               maxTessellationPatchSize;
    uint32_t               maxTessellationControlPerVertexInputComponents;
    uint32_t               maxTessellationControlPerVertexOutputComponents;
    uint32_t               maxTessellationControlPerPatchOutputComponents;
    uint32_t               maxTessellationControlTotalOutputComponents;
    uint32_t               maxTessellationEvaluationInputComponents;
    uint32_t               maxTessellationEvaluationOutputComponents;
    uint32_t               maxGeometryShaderInvocations;
    uint32_t               maxGeometryInputComponents;
    uint32_t               maxGeometryOutputComponents;
    uint32_t               maxGeometryOutputVertices;
    uint32_t               maxGeometryTotalOutputComponents;
    uint32_t               maxFragmentInputComponents;
    uint32_t               maxFragmentOutputAttachments;
    uint32_t               maxFragmentDualSrcAttachments;
    uint32_t               maxFragmentCombinedOutputResources;
    uint32_t               maxComputeSharedMemorySize;
    std::array<uint32_t,3> maxComputeWorkGroupCount;
    uint32_t               maxComputeWorkGroupInvocations;
    std::array<uint32_t,3> maxComputeWorkGroupSize;
    uint32_t               subPixelPrecisionBits;
    uint32_t               subTexelPrecisionBits;
    uint32_t               mipmapPrecisionBits;
    uint32_t               maxDrawIndexedIndexValue;
    uint32_t               maxDrawIndirectCount;
    float                  maxSamplerLodBias;
    float                  maxSamplerAnisotropy;
    uint32_t               maxViewports;
    std::array<uint32_t,2> maxViewportDimensions;
    std::array<float,2>    viewportBoundsRange;
    uint32_t               viewportSubPixelBits;
    size_t                 minMemoryMapAlignment;
    uint64_t               minTexelBufferOffsetAlignment;
    uint64_t               minUniformBufferOffsetAlignment;
    uint64_t               minStorageBufferOffsetAlignment;
    int32_t                minTexelOffset;
    uint32_t               maxTexelOffset;
    int32_t                minTexelGatherOffset;
    uint32_t               maxTexelGatherOffset;
    float                  minInterpolationOffset;
    float                  maxInterpolationOffset;
    uint32_t               subPixelInterpolationOffsetBits;
    uint32_t               maxFramebufferWidth;
    uint32_t               maxFramebufferHeight;
    uint32_t               maxFramebufferLayers;
    int32_t                framebufferColorSampleCounts;
    int32_t                framebufferDepthSampleCounts;
    int32_t                framebufferStencilSampleCounts;
    int32_t                framebufferNoAttachmentsSampleCounts;
    uint32_t               maxColorAttachments;
    int32_t                sampledImageColorSampleCounts;
    int32_t                sampledImageIntegerSampleCounts;
    int32_t                sampledImageDepthSampleCounts;
    int32_t                sampledImageStencilSampleCounts;
    int32_t                storageImageSampleCounts;
    uint32_t               maxSampleMaskWords;
    bool                   timestampComputeAndGraphics;
    float                  timestampPeriod;
    uint32_t               maxClipDistances;
    uint32_t               maxCullDistances;
    uint32_t               maxCombinedClipAndCullDistances;
    uint32_t               discreteQueuePriorities;
    std::array<float,2>    pointSizeRange;
    std::array<float,2>    lineWidthRange;
    float                  pointSizeGranularity;
    float                  lineWidthGranularity;
    bool                   strictLines;
    bool                   standardSampleLocations;
    uint64_t               optimalBufferCopyOffsetAlignment;
    uint64_t               optimalBufferCopyRowPitchAlignment;
    uint64_t               nonCoherentAtomSize;
    
    PhysicalDeviceLimits() {}
    PhysicalDeviceLimits( const PhysicalDeviceLimits& limits );

private:

    PhysicalDeviceLimits( const VkPhysicalDeviceLimits& limits );
};

struct PhysicalDeviceQueueFamilyProperties
{
    friend class Instance;
    friend class PhysicalDeviceInfo;
    
public:
    
    bool     graphics;
    bool     compute;
    bool     transfer;
    bool     sparse;
    uint32_t count;

    PhysicalDeviceQueueFamilyProperties() {}
    PhysicalDeviceQueueFamilyProperties(
        const PhysicalDeviceQueueFamilyProperties& props
        );

private:
    
    uint32_t   timestampValidBits;
    VkExtent3D minImageTransferGranularity;

    PhysicalDeviceQueueFamilyProperties( const VkQueueFamilyProperties& props );
};

struct PhysicalDeviceInfo
{
    friend class Instance;

public:

    PhysicalDeviceType                         type = PhysicalDeviceType::OTHER;
    std::string                                name;
    std::vector<PhysicalDeviceQueueFamilyProperties> queueFamilyInfo;

private:

    PhysicalDeviceInfo( Instance& instance, VkPhysicalDevice physical );

    uint32_t               vendorID;
    uint32_t               deviceID;
    PhysicalDeviceLimits   limits;
    PhysicalDeviceFeatures features;
};
