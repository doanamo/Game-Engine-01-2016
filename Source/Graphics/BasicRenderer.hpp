#pragma once

#include "Precompiled.hpp"
#include "Buffer.hpp"
#include "VertexInput.hpp"
#include "Sampler.hpp"
#include "Shader.hpp"

//
// Basic Renderer
//
//  Handles basic drawing routines.
//

namespace Graphics
{
    // Forward declarations.
    class Texture;

    // Clear flags.
    struct ClearFlags
    {
        enum Type
        {
            None    = 0,
            Color   = 1 << 0,
            Depth   = 1 << 1,
            Stencil = 1 << 2,
            All     = ~0,
        };
    };

    // Basic renderer class.
    class BasicRenderer
    {
    public:
        // Sprite structure.
        struct Sprite
        {
            struct Info
            {
                Info();

                bool operator==(const Info& right) const;
                bool operator!=(const Info& right) const;

                const Texture* texture;
                bool transparent;
                bool filter;
            } info;
            
            struct Data
            {
                Data();

                glm::mat4 transform;
                glm::vec4 rectangle;
                glm::vec4 color;
            } data;
        };

        // Type declarations.
        typedef std::shared_ptr<const Shader> ShaderPtr;
        typedef std::vector<Sprite::Info> SpriteInfoList;
        typedef std::vector<Sprite::Data> SpriteDataList;

        // Constant variables.
        static const int SpriteBatchSize = 128;

    public:
        BasicRenderer();
        ~BasicRenderer();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the basic renderer instance.
        bool Initialize(Context& context);

        // Clears the frame buffer.
        void Clear(uint32_t flags = ClearFlags::All);

        // Draws sprites.
        void DrawSprites(const SpriteInfoList& spriteInfo, const SpriteDataList& spriteData, const glm::mat4& transform);

        // Sets the clear color.
        void SetClearColor(const glm::vec4& color);

        // Sets the clear depth.
        void SetClearDepth(float depth);

        // Sets the stencil depth.
        void SetClearStencil(int stencil);

    private:
        // Graphics objects.
        VertexBuffer   m_vertexBuffer;
        InstanceBuffer m_instanceBuffer;
        VertexInput    m_vertexInput;
        Sampler        m_nearestSampler;
        Sampler        m_linearSampler;
        ShaderPtr      m_shader;
        
        // Initialization state.
        bool m_initialized;
    };
}
