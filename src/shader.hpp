#pragma once

#include <array>
#include <fstream>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "device.hpp"

class GraphicsShader
{
public:
    
    GraphicsShader() {}

    GraphicsShader( Device*              device,
                    std::vector<uint8_t> vertexCode,
                    std::vector<uint8_t> fragmentCode,
                    std::vector<uint8_t> tessctrlCode,
                    std::vector<uint8_t> tessevalCode,
                    std::vector<uint8_t> geometryCode )
    {
        this->init( device, vertexCode, fragmentCode, tessctrlCode,
                    tessevalCode, geometryCode );
    }

    ~GraphicsShader() { this->deinit(); }

    void init( Device*              device,
               std::vector<uint8_t> vertexCode,
               std::vector<uint8_t> fragmentCode,
               std::vector<uint8_t> tessctrlCode,
               std::vector<uint8_t> tessevalCode,
               std::vector<uint8_t> geometryCode );

    void deinit();

    uint32_t getNumModules() const;

    const std::array<VkPipelineShaderStageCreateInfo, 5>& getPipelineInfo() const;

private:

    Device*                                        device = nullptr;
    uint32_t                                       numModules;

    std::array<VkShaderModule, 5>                  modules;
    std::array<VkPipelineShaderStageCreateInfo, 5> pipelineInfo;

    void createShaderModule( VkShaderStageFlagBits stage,
                             std::vector<uint8_t>  code );
};
