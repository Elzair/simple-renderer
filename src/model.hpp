#ifndef __MODEL_HPP
#define __MODEL_HPP

#include "base-includes.hpp"
#include "buffer.hpp"

/**
 * Vertex Code
 */

struct Vertex
{
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  static VkVertexInputBindingDescription getBindingDescription(  )
  {
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof( Vertex );
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions(  )
  {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset   = offsetof( Vertex, pos );

    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset   = offsetof( Vertex, color );

    attributeDescriptions[2].binding  = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset   = offsetof( Vertex, texCoord );

    return attributeDescriptions;
  }

  bool operator==( const Vertex& other ) const
  {
    return this->pos == other.pos &&
           this->color == other.color &&
           this->texCoord == other.texCoord;
  }
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
           std::string      fileName )
    {
        this->init( physical,
                    device,
                    queue,
                    commandPool,
                    fileName );
    }

    Model(  ) { }

    ~Model(  )
    {
        this->deinit();
    }

    void init( VkPhysicalDevice physical,
               VkDevice         device,
               VkQueue          queue,
               VkCommandPool    commandPool,
               std::string      fileName )
    {
        tinyobj::attrib_t                attrib;
        std::vector<tinyobj::shape_t>    shapes;
        std::vector<tinyobj::material_t> materials;
        std::string                      err;

        if ( !tinyobj::LoadObj( &attrib, &shapes,
                                &materials, &err,
                                fileName.c_str() ) )
        {
            throw std::runtime_error( err );
        }

        std::vector<Vertex>              vertices;
        std::unordered_map<Vertex, int>  uniqueVertices = {};
        std::vector<uint32_t>            indices;

        for ( const auto& shape : shapes )
        {
            for ( const auto& index : shape.mesh.indices )
            {
                Vertex vertex = {};

                vertex.pos = {
                    attrib.vertices[ 3 * index.vertex_index + 0 ],
                    attrib.vertices[ 3 * index.vertex_index + 1 ],
                    attrib.vertices[ 3 * index.vertex_index + 2 ]
                };

                vertex.texCoord = {
                    attrib.texcoords[ 2 * index.texcoord_index + 0 ],
                    1.0f - attrib.texcoords[ 2 * index.texcoord_index + 1 ]
                };

                if ( uniqueVertices.count( vertex ) == 0 )
                {
                    uniqueVertices[vertex] = vertices.size();
                    vertices.push_back( vertex );
                }

                indices.push_back( uniqueVertices[vertex] );
            }
        }

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        this->vertexBuffer.init( physical,
                                 device,
                                 queue,
                                 commandPool,
                                 bufferSize,
                                 BufferUsage::VERTEX );
        this->vertexBuffer.copy( (void*)vertices.data(),
                                 true,
                                 bufferSize );

        this->indexSize = sizeof(indices[0]) * indices.size();
        this->indexBuffer.init( physical,
                                device,
                                queue,
                                commandPool,
                                this->indexSize,
                                BufferUsage::INDEX );
        this->indexBuffer.copy( (void*)indices.data(),
                                true,
                                this->indexSize );
    }

    void deinit(  )
    {
        this->indexBuffer.deinit();
        this->vertexBuffer.deinit();
    }
};

#endif
