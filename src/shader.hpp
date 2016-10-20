#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <fstream>
#include "base-includes.hpp"

static std::vector<char> readFile( const std::string& filename )
{
  std::ifstream file( filename, std::ios::ate | std::ios::binary );

  if (!file.is_open())
  {
    throw std::runtime_error( "Failed to open SPIRV file!" );
  }

  size_t fileSize = (size_t) file.tellg();
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
  
  if ( vkCreateShaderModule( device, &shaderCreateInfo,
                             nullptr, &shaderModule ) != VK_SUCCESS )
  {
    throw std::runtime_error( "Failed to create shader module!" );
  }

  return shaderModule;
}

#endif
