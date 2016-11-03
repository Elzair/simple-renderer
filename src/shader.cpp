#include <iostream>
#include <fstream>
#include <vector>

#include <vulkan/vulkan.h>

#include "common.hpp"
#include "shader.hpp"

std::vector<char> readFile( const std::string& filename )
{
    std::ifstream file( filename, std::ios::ate | std::ios::binary );

    assert( file.is_open() );

    std::size_t fileSize = (std::size_t) file.tellg();
    std::vector<char> buffer( fileSize );

    file.seekg( 0 );
    file.read( buffer.data(), fileSize );
    file.close();

    return buffer;
}

VkShaderModule createShaderModule(
    VkDevice                 device,
    const std::vector<char>& code
    )
{
    VkShaderModuleCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = code.size();
    shaderCreateInfo.pCode    = (uint32_t*) code.data();

    VkShaderModule shaderModule = VK_NULL_HANDLE;
  
    VK_CHECK_RESULT( vkCreateShaderModule( device,
                                           &shaderCreateInfo,
                                           nullptr,
                                           &shaderModule ) );

    return shaderModule;
}
