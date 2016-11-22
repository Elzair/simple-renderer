#pragma once

#include <array>
#include <fstream>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

class GraphicsShader
{
public:

    std::uint32_t                                  num_modules;
    std::array<VkShaderModule, 5>                  modules;
    std::array<VkPipelineShaderStageCreateInfo, 5> pipeline_info;
    
    GraphicsShader() {}

    GraphicsShader( VkDevice             device,
                    std::vector<uint8_t> vertex_code,
                    std::vector<uint8_t> fragment_code,
                    std::vector<uint8_t> tessctrl_code,
                    std::vector<uint8_t> tesseval_code,
                    std::vector<uint8_t> geometry_code )
    {
        this->init( device, vertex_code, fragment_code, tessctrl_code,
                    tesseval_code, geometry_code );
    }

    ~GraphicsShader() { this->deinit(); }

    void init( VkDevice             device,
               std::vector<uint8_t> vertex_code,
               std::vector<uint8_t> fragment_code,
               std::vector<uint8_t> tessctrl_code,
               std::vector<uint8_t> tesseval_code,
               std::vector<uint8_t> geometry_code );

    void deinit();

private:

    VkDevice device;

    void createShaderModule( VkShaderStageFlagBits stage,
                             std::vector<uint8_t>  code );
};
