#include <iostream>
#include <fstream>
#include <vector>

#include <vulkan/vulkan.h>

#include "common.hpp"
#include "shader.hpp"
#include "utils.hpp"

void GraphicsShader::init( Device*              device,
                           std::vector<uint8_t> vertexCode,
                           std::vector<uint8_t> fragmentCode,
                           std::vector<uint8_t> tessctrlCode,
                           std::vector<uint8_t> tessevalCode,
                           std::vector<uint8_t> geometryCode )
{
    this->numModules = 0;
    this->device     = device;

    this->createShaderModule( VK_SHADER_STAGE_VERTEX_BIT,
                              vertexCode );
    this->createShaderModule( VK_SHADER_STAGE_FRAGMENT_BIT,
                              fragmentCode );
    this->createShaderModule( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
                              tessctrlCode );
    this->createShaderModule( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                              tessevalCode );
    this->createShaderModule( VK_SHADER_STAGE_GEOMETRY_BIT,
                              geometryCode );
}

void GraphicsShader::deinit()
{
    for ( auto i = 0; i < this->numModules; i++ )
    {
        if ( this->modules[ i ] != VK_NULL_HANDLE )
        {
            this->device->destroyShaderModule( this->modules[ i ] );
        }
    }
}

uint32_t GraphicsShader::getNumModules() const
{
    return this->numModules;
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
                             &this->modules[ this->numModules ]
                             ) );

        auto& ps_info  = this->pipelineInfo[ this->numModules ];
        ps_info = {};
        ps_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ps_info.stage  = stage;
        ps_info.module = this->modules[ this->numModules ];
        ps_info.pName  = "main";
        
        this->numModules++;
    }
}
