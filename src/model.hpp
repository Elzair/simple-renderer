#ifndef __MODEL_HPP
#define __MODEL_HPP

//#include "base-includes.hpp"
#include <string>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifdef GLM_HAS_CXX11_STL   // Hack to get GLM's
#undef GLM_HAS_CXX11_STL  // hash support working
#endif
#define GLM_HAS_CXX11_STL 1
#include <glm/gtx/hash.hpp>

#include "buffer.hpp"

/*
 * Vertex Code
 */

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription(  );

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions(  );

    bool operator==( const Vertex& other ) const;
};

namespace std
{
    template<> struct hash<Vertex>
    {
        size_t operator()( Vertex const& vertex ) const
        {
            return ( ( hash<glm::vec3>()( vertex.pos ) ^
                       ( hash<glm::vec3>()( vertex.color ) << 1 )) >> 1 ) ^
                ( hash<glm::vec2>()( vertex.texCoord ) << 1 );
        }
    };
}

/*
 * Model Code
 */

class Model
{
public:
    
    Buffer      vertexBuffer;
    Buffer      indexBuffer;
    std::size_t indexSize;

    Model( VkPhysicalDevice physical,
           VkDevice         device,
           VkQueue          queue,
           VkCommandPool    commandPool,
           std::string      fileName );

    Model(  );

    ~Model(  );

    void init( VkPhysicalDevice physical,
               VkDevice         device,
               VkQueue          queue,
               VkCommandPool    commandPool,
               std::string      fileName );

    void deinit(  );
};

#endif
