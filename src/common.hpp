#pragma once

#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

/**
 * Common Struct Declarations
 */

typedef VkExtent2D Extent2D;
typedef VkExtent3D Extent3D;
typedef VkOffset2D Offset2D;
typedef VkOffset3D Offset3D;

/**
 * Common Enum Declarations
 */

enum class PhysicalDeviceType : int32_t
{
    OTHER      = 0,
    INTEGRATED = 1,
    DISCRETE   = 2,
    VIRTUAL    = 3,
    CPU        = 4
};

enum class CompareOp : int32_t
{
    NEVER            = 0,
    LESS             = 1,
    EQUAL            = 2,
    LESS_OR_EQUAL    = 3,
    GREATER          = 4,
    NOT_EQUAL        = 5,
    GREATER_OR_EQUAL = 6,
    ALWAYS           = 7
};

enum class StencilOp : int32_t
{
    KEEP                = 0,
    ZERO                = 1,
    REPLACE             = 2,
    INCREMENT_AND_CLAMP = 3,
    DECREMENT_AND_CLAMP = 4,
    INVERT              = 5,
    INCREMENT_AND_WRAP  = 6,
    DECREMENT_AND_WRAP  = 7
};

enum class LogicOp : int32_t
{
    CLEAR         = 0,
    AND           = 1,
    AND_REVERSE   = 2,
    COPY          = 3,
    AND_INVERTED  = 4,
    NO_OP         = 5,
    XOR           = 6,
    OR            = 7,
    NOR           = 8,
    EQUIVALENT    = 9,
    INVERT        = 10,
    OR_REVERSE    = 11,
    COPY_INVERTED = 12,
    OR_INVERTED   = 13,
    NAND          = 14,
    SET           = 15
};

enum class BlendFactor : int32_t
{
    ZERO                     = 0,
    ONE                      = 1,
    SRC_COLOR                = 2,
    ONE_MINUS_SRC_COLOR      = 3,
    DST_COLOR                = 4,
    ONE_MINUS_DST_COLOR      = 5,
    SRC_ALPHA                = 6,
    ONE_MINUS_SRC_ALPHA      = 7,
    DST_ALPHA                = 8,
    ONE_MINUS_DST_ALPHA      = 9,
    CONSTANT_COLOR           = 10,
    ONE_MINUS_CONSTANT_COLOR = 11,
    CONSTANT_ALPHA           = 12,
    ONE_MINUS_CONSTANT_ALPHA = 13,
    SRC_ALPHA_SATURATE       = 14,
    SRC1_COLOR               = 15,
    ONE_MINUS_SRC1_COLOR     = 16,
    SRC1_ALPHA               = 17,
    ONE_MINUS_SRC1_ALPHA     = 18
};

enum class Filter : int32_t
{
    NEAREST   = 0,
    LINEAR    = 1,
    CUBIC_IMG = 1000015000
};

enum class SamplerMipmapMode : int32_t
{
    NEAREST = 0,
    LINEAR  = 1
};

enum class SamplerAddressMode : int32_t
{
    REPEAT               = 0,
    MIRRORED_REPEAT      = 1,
    CLAMP_TO_EDGE        = 2,
    CLAMP_TO_BORDER      = 3,
    MIRROR_CLAMP_TO_EDGE = 4
};

enum class Format : int32_t
{
    UNDEFINED                   = 0,
    R4G4_UNORM_PACK8            = 1,
    R4G4B4A4_UNORM_PACK16       = 2,
    B4G4R4A4_UNORM_PACK16       = 3,
    R5G6B5_UNORM_PACK16         = 4,
    B5G6R5_UNORM_PACK16         = 5,
    R5G5B5A1_UNORM_PACK16       = 6,
    B5G5R5A1_UNORM_PACK16       = 7,
    A1R5G5B5_UNORM_PACK16       = 8,
    R8_UNORM                    = 9,
    R8_SNORM                    = 10,
    R8_USCALED                  = 11,
    R8_SSCALED                  = 12,
    R8_UINT                     = 13,
    R8_SINT                     = 14,
    R8_SRGB                     = 15,
    R8G8_UNORM                  = 16,
    R8G8_SNORM                  = 17,
    R8G8_USCALED                = 18,
    R8G8_SSCALED                = 19,
    R8G8_UINT                   = 20,
    R8G8_SINT                   = 21,
    R8G8_SRGB                   = 22,
    R8G8B8_UNORM                = 23,
    R8G8B8_SNORM                = 24,
    R8G8B8_USCALED              = 25,
    R8G8B8_SSCALED              = 26,
    R8G8B8_UINT                 = 27,
    R8G8B8_SINT                 = 28,
    R8G8B8_SRGB                 = 29,
    B8G8R8_UNORM                = 30,
    B8G8R8_SNORM                = 31,
    B8G8R8_USCALED              = 32,
    B8G8R8_SSCALED              = 33,
    B8G8R8_UINT                 = 34,
    B8G8R8_SINT                 = 35,
    B8G8R8_SRGB                 = 36,
    R8G8B8A8_UNORM              = 37,
    R8G8B8A8_SNORM              = 38,
    R8G8B8A8_USCALED            = 39,
    R8G8B8A8_SSCALED            = 40,
    R8G8B8A8_UINT               = 41,
    R8G8B8A8_SINT               = 42,
    R8G8B8A8_SRGB               = 43,
    B8G8R8A8_UNORM              = 44,
    B8G8R8A8_SNORM              = 45,
    B8G8R8A8_USCALED            = 46,
    B8G8R8A8_SSCALED            = 47,
    B8G8R8A8_UINT               = 48,
    B8G8R8A8_SINT               = 49,
    B8G8R8A8_SRGB               = 50,
    A8B8G8R8_UNORM_PACK32       = 51,
    A8B8G8R8_SNORM_PACK32       = 52,
    A8B8G8R8_USCALED_PACK32     = 53,
    A8B8G8R8_SSCALED_PACK32     = 54,
    A8B8G8R8_UINT_PACK32        = 55,
    A8B8G8R8_SINT_PACK32        = 56,
    A8B8G8R8_SRGB_PACK32        = 57,
    A2R10G10B10_UNORM_PACK32    = 58,
    A2R10G10B10_SNORM_PACK32    = 59,
    A2R10G10B10_USCALED_PACK32  = 60,
    A2R10G10B10_SSCALED_PACK32  = 61,
    A2R10G10B10_UINT_PACK32     = 62,
    A2R10G10B10_SINT_PACK32     = 63,
    A2B10G10R10_UNORM_PACK32    = 64,
    A2B10G10R10_SNORM_PACK32    = 65,
    A2B10G10R10_USCALED_PACK32  = 66,
    A2B10G10R10_SSCALED_PACK32  = 67,
    A2B10G10R10_UINT_PACK32     = 68,
    A2B10G10R10_SINT_PACK32     = 69,
    R16_UNORM                   = 70,
    R16_SNORM                   = 71,
    R16_USCALED                 = 72,
    R16_SSCALED                 = 73,
    R16_UINT                    = 74,
    R16_SINT                    = 75,
    R16_SFLOAT                  = 76,
    R16G16_UNORM                = 77,
    R16G16_SNORM                = 78,
    R16G16_USCALED              = 79,
    R16G16_SSCALED              = 80,
    R16G16_UINT                 = 81,
    R16G16_SINT                 = 82,
    R16G16_SFLOAT               = 83,
    R16G16B16_UNORM             = 84,
    R16G16B16_SNORM             = 85,
    R16G16B16_USCALED           = 86,
    R16G16B16_SSCALED           = 87,
    R16G16B16_UINT              = 88,
    R16G16B16_SINT              = 89,
    R16G16B16_SFLOAT            = 90,
    R16G16B16A16_UNORM          = 91,
    R16G16B16A16_SNORM          = 92,
    R16G16B16A16_USCALED        = 93,
    R16G16B16A16_SSCALED        = 94,
    R16G16B16A16_UINT           = 95,
    R16G16B16A16_SINT           = 96,
    R16G16B16A16_SFLOAT         = 97,
    R32_UINT                    = 98,
    R32_SINT                    = 99,
    R32_SFLOAT                  = 100,
    R32G32_UINT                 = 101,
    R32G32_SINT                 = 102,
    R32G32_SFLOAT               = 103,
    R32G32B32_UINT              = 104,
    R32G32B32_SINT              = 105,
    R32G32B32_SFLOAT            = 106,
    R32G32B32A32_UINT           = 107,
    R32G32B32A32_SINT           = 108,
    R32G32B32A32_SFLOAT         = 109,
    R64_UINT                    = 110,
    R64_SINT                    = 111,
    R64_SFLOAT                  = 112,
    R64G64_UINT                 = 113,
    R64G64_SINT                 = 114,
    R64G64_SFLOAT               = 115,
    R64G64B64_UINT              = 116,
    R64G64B64_SINT              = 117,
    R64G64B64_SFLOAT            = 118,
    R64G64B64A64_UINT           = 119,
    R64G64B64A64_SINT           = 120,
    R64G64B64A64_SFLOAT         = 121,
    B10G11R11_UFLOAT_PACK32     = 122,
    E5B9G9R9_UFLOAT_PACK32      = 123,
    D16_UNORM                   = 124,
    X8_D24_UNORM_PACK32         = 125,
    D32_SFLOAT                  = 126,
    S8_UINT                     = 127,
    D16_UNORM_S8_UINT           = 128,
    D24_UNORM_S8_UINT           = 129,
    D32_SFLOAT_S8_UINT          = 130,
    BC1_RGB_UNORM_BLOCK         = 131,
    BC1_RGB_SRGB_BLOCK          = 132,
    BC1_RGBA_UNORM_BLOCK        = 133,
    BC1_RGBA_SRGB_BLOCK         = 134,
    BC2_UNORM_BLOCK             = 135,
    BC2_SRGB_BLOCK              = 136,
    BC3_UNORM_BLOCK             = 137,
    BC3_SRGB_BLOCK              = 138,
    BC4_UNORM_BLOCK             = 139,
    BC4_SNORM_BLOCK             = 140,
    BC5_UNORM_BLOCK             = 141,
    BC5_SNORM_BLOCK             = 142,
    BC6H_UFLOAT_BLOCK           = 143,
    BC6H_SFLOAT_BLOCK           = 144,
    BC7_UNORM_BLOCK             = 145,
    BC7_SRGB_BLOCK              = 146,
    ETC2_R8G8B8_UNORM_BLOCK     = 147,
    ETC2_R8G8B8_SRGB_BLOCK      = 148,
    ETC2_R8G8B8A1_UNORM_BLOCK   = 149,
    ETC2_R8G8B8A1_SRGB_BLOCK    = 150,
    ETC2_R8G8B8A8_UNORM_BLOCK   = 151,
    ETC2_R8G8B8A8_SRGB_BLOCK    = 152,
    EAC_R11_UNORM_BLOCK         = 153,
    EAC_R11_SNORM_BLOCK         = 154,
    EAC_R11G11_UNORM_BLOCK      = 155,
    EAC_R11G11_SNORM_BLOCK      = 156,
    ASTC_4x4_UNORM_BLOCK        = 157,
    ASTC_4x4_SRGB_BLOCK         = 158,
    ASTC_5x4_UNORM_BLOCK        = 159,
    ASTC_5x4_SRGB_BLOCK         = 160,
    ASTC_5x5_UNORM_BLOCK        = 161,
    ASTC_5x5_SRGB_BLOCK         = 162,
    ASTC_6x5_UNORM_BLOCK        = 163,
    ASTC_6x5_SRGB_BLOCK         = 164,
    ASTC_6x6_UNORM_BLOCK        = 165,
    ASTC_6x6_SRGB_BLOCK         = 166,
    ASTC_8x5_UNORM_BLOCK        = 167,
    ASTC_8x5_SRGB_BLOCK         = 168,
    ASTC_8x6_UNORM_BLOCK        = 169,
    ASTC_8x6_SRGB_BLOCK         = 170,
    ASTC_8x8_UNORM_BLOCK        = 171,
    ASTC_8x8_SRGB_BLOCK         = 172,
    ASTC_10x5_UNORM_BLOCK       = 173,
    ASTC_10x5_SRGB_BLOCK        = 174,
    ASTC_10x6_UNORM_BLOCK       = 175,
    ASTC_10x6_SRGB_BLOCK        = 176,
    ASTC_10x8_UNORM_BLOCK       = 177,
    ASTC_10x8_SRGB_BLOCK        = 178,
    ASTC_10x10_UNORM_BLOCK      = 179,
    ASTC_10x10_SRGB_BLOCK       = 180,
    ASTC_12x10_UNORM_BLOCK      = 181,
    ASTC_12x10_SRGB_BLOCK       = 182,
    ASTC_12x12_UNORM_BLOCK      = 183,
    ASTC_12x12_SRGB_BLOCK       = 184,
    PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
    PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
    PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
    PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
    PVRTC1_2BPP_SRGB_BLOCK_IMG  = 1000054004,
    PVRTC1_4BPP_SRGB_BLOCK_IMG  = 1000054005,
    PVRTC2_2BPP_SRGB_BLOCK_IMG  = 1000054006,
    PVRTC2_4BPP_SRGB_BLOCK_IMG  = 1000054007
};

enum class DescriptorType : int32_t
{
    SAMPLER                = 0,
    COMBINED_IMAGE_SAMPLER = 1,
    SAMPLED_IMAGE          = 2,
    STORAGE_IMAGE          = 3,
    UNIFORM_TEXEL_BUFFER   = 4,
    STORAGE_TEXEL_BUFFER   = 5,
    UNIFORM_BUFFER         = 6,
    STORAGE_BUFFER         = 7,
    UNIFORM_BUFFER_DYNAMIC = 8,
    STORAGE_BUFFER_DYNAMIC = 9,
    INPUT_ATTACHMENT       = 10
};


/**
 * Miscellaneous
 */

static inline void CheckResult( VkResult    res,
                                const char* file,
                                const char* func,
                                int         line )
{
    if ( res != VK_SUCCESS )
    {
        std::cerr << "Error in " << file << " " << func << " " << line << ": VkResult is \"";
        switch( res )
        {
        case VK_NOT_READY:
            std::cerr << "NOT_READY";
            break;
        case VK_TIMEOUT:
            std::cerr << "TIMEOUT";
            break;
        case VK_EVENT_SET:
            std::cerr << "EVENT_SET";
            break;
        case VK_EVENT_RESET:
            std::cerr << "EVENT_RESET";
            break;
        case VK_INCOMPLETE:
            std::cerr << "INCOMPLETE";
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            std::cerr << "ERROR_OUT_OF_HOST_MEMORY";
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            std::cerr << "ERROR_OUT_OF_DEVICE_MEMORY";
            break;
        case VK_ERROR_INITIALIZATION_FAILED:
            std::cerr << "ERROR_INITIALIZATION_FAILED";
            break;
        case VK_ERROR_DEVICE_LOST:
            std::cerr << "ERROR_DEVICE_LOST";
            break;
        case VK_ERROR_MEMORY_MAP_FAILED:
            std::cerr << "ERROR_MEMORY_MAP_FAILED";
            break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            std::cerr << "ERROR_LAYER_NOT_PRESENT";
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            std::cerr << "ERROR_EXTENSION_NOT_PRESENT";
            break;
        case VK_ERROR_FEATURE_NOT_PRESENT:
            std::cerr << "ERROR_FEATURE_NOT_PRESENT";
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            std::cerr << "ERROR_INCOMPATIBLE_DRIVER";
            break;
        case VK_ERROR_TOO_MANY_OBJECTS:
            std::cerr << "ERROR_TOO_MANY_OBJECTS";
            break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            std::cerr << "ERROR_FORMAT_NOT_SUPPORTED";
            break;
        case VK_ERROR_SURFACE_LOST_KHR:
            std::cerr << "ERROR_SURFACE_LOST_KHR";
            break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            std::cerr << "ERROR_NATIVE_WINDOW_IN_USE_KHR";
            break;
        case VK_SUBOPTIMAL_KHR:
            std::cerr << "SUBOPTIMAL_KHR";
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            std::cerr << "ERROR_OUT_OF_DATE_KHR";
            break;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            std::cerr << "ERROR_INCOMPATIBLE_DISPLAY_KHR";
            break;
        case VK_ERROR_VALIDATION_FAILED_EXT:
            std::cerr << "ERROR_VALIDATION_FAILED_EXT";
            break;
        case VK_ERROR_INVALID_SHADER_NV:
            std::cerr << "ERROR_INVALID_SHADER_NV";
            break;
        default:
            std::cerr << "Unknown Error";
            break;
        }
        std::cerr << "\"" << std::endl;
    }

    assert( res == VK_SUCCESS );
}

const std::vector<const char*> requiredDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#if defined( DEBUG_BUILD )

const bool enableValidationLayers = true;

const std::vector<const char*> requiredValidationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

#define VK_CHECK_RESULT( res ) CheckResult( res, __FILE__, __func__, __LINE__ )

#else

const bool enableValidationLayers = false;

const std::vector<const char*> requiredValidationLayers = {};

#define VK_CHECK_RESULT( res ) assert( ( res ) == VK_SUCCESS )

#endif
