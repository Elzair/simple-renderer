#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <fstream>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

std::vector<char> readFile( const std::string& filename );

VkShaderModule createShaderModule(
    VkDevice                 device,
    const std::vector<char>& code
    );

#endif
