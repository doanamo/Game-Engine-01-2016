#pragma once

#include "Precompiled.hpp"

//
// Vertex Input
//
//  Creates a vertex layout that binds buffers to the shader inputs on the pipeline.
//
//  Creating and using a vertex input:
//      const Graphics::VertexAttribute vertexAttributes[] =
//      {
//          { &vertexBuffer, Graphics::VertexAttributeTypes::Float2 }, // Position
//          { &vertexBuffer, Graphics::VertexAttributeTypes::Float2 }, // Texture
//          { &vertexBuffer, Graphics::VertexAttributeTypes::Float4 }, // Color
//      };
//      
//      Graphics::VertexInput vertexInput;
//      vertexInput.Initialize(boost::size(vertexAttributes), &vertexAttributes[0]);
//      
//      glBindVertexArray(vertexInput.GetHandle());
//

namespace Graphics
{
    // Forward declarations.
    class Buffer;

    // Vertex attribute types.
    enum class VertexAttributeTypes
    {
        Invalid,

        Float1,
        Float2,
        Float3,
        Float4,

        Float4x4,

        Count,
    };

    // Vertex attribute structure.
    struct VertexAttribute
    {
        const Buffer* buffer;
        VertexAttributeTypes type;
    };

    // Vertex input class.
    class VertexInput
    {
    public:
        VertexInput();
        ~VertexInput();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the vertex input instance.
        bool Initialize(int attributeCount, const VertexAttribute* attributes);

        // Gets the vertex array object handle.
        GLuint GetHandle() const
        {
            return m_handle;
        }

        // Checks if instance is valid.
        bool IsValid() const
        {
            return m_initialized;
        }

    private:
        // Vertex array object.
        GLuint m_handle;

        // Initialization state.
        bool m_initialized;
    };
}
