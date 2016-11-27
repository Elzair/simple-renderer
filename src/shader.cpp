#include <iostream>
#include <fstream>
#include <vector>

#include <vulkan/vulkan.h>

#include "common.hpp"
#include "shader.hpp"
#include "utils.hpp"

void GraphicsShader::init( Device*              device,
                           std::vector<uint8_t> vertex_code,
                           std::vector<uint8_t> fragment_code,
                           std::vector<uint8_t> tessctrl_code,
                           std::vector<uint8_t> tesseval_code,
                           std::vector<uint8_t> geometry_code )
{
    this->num_modules = 0;
    this->device      = device;

    this->createShaderModule( VK_SHADER_STAGE_VERTEX_BIT,
                              vertex_code );
    this->createShaderModule( VK_SHADER_STAGE_FRAGMENT_BIT,
                              fragment_code );
    this->createShaderModule( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
                              tessctrl_code );
    this->createShaderModule( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                              tesseval_code );
    this->createShaderModule( VK_SHADER_STAGE_GEOMETRY_BIT,
                              geometry_code );
}

void GraphicsShader::deinit()
{
    for ( auto i = 0; i < this->num_modules; i++ )
    {
        if ( this->modules[ i ] != VK_NULL_HANDLE )
        {
            this->device->destroyShaderModule( this->modules[ i ] );
        }
    }
}

void GraphicsShader::createShaderModule(
    VkShaderStageFlagBits stage,
    std::vector<uint8_t>  code 
    )
{
    if ( code.size() > 0 ) // Only create a shader if the user specified code
    {
        VkShaderModuleCreateInfo info = {};
        info.sType     = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = code.size();
        info.pCode    = (uint32_t*)code.data();

        VK_CHECK_RESULT( this->device->createShaderModule(
                             &info,
                             &this->modules[ this->num_modules ]
                             ) );

        auto& ps_info  = this->pipeline_info[ this->num_modules ];
        ps_info = {};
        ps_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ps_info.stage  = stage;
        ps_info.module = this->modules[ this->num_modules ];
        ps_info.pName  = "main";
        
        this->num_modules++;
    }
}
