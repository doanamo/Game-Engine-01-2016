#include "Precompiled.hpp"
#include "BasicRenderer.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/Texture.hpp"
using namespace Graphics;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the basic renderer! "

    // Vertex structure.
    struct Vertex
    {
        glm::vec2 position;
        glm::vec2 texture;
    };
}

BasicRenderer::Sprite::Info::Info() :
    texture(nullptr),
    transparent(false),
    filter(true)
{
}

bool BasicRenderer::Sprite::Info::operator==(const Info& right) const
{
    return this->texture == right.texture && this->transparent == right.transparent && this->filter == right.filter;
}

bool BasicRenderer::Sprite::Info::operator!=(const Info& right) const
{
    return !(*this == right);
}

BasicRenderer::Sprite::Data::Data() :
    transform(1.0f),
    rectangle(0.0f, 0.0f, 1.0f, 1.0f),
    color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

BasicRenderer::BasicRenderer()
{
}

BasicRenderer::~BasicRenderer()
{
    this->Cleanup();
}

void BasicRenderer::Cleanup()
{
    if(!m_initialized)
        return;

    // Cleanup graphics objects.
    m_vertexBuffer.Cleanup();
    m_instanceBuffer.Cleanup();
    m_vertexInput.Cleanup();
    m_nearestSampler.Cleanup();
    m_linearSampler.Cleanup();

    m_shader = nullptr;

    // Reset initialization state.
    m_initialized = false;
}

bool BasicRenderer::Initialize(Context& context)
{
    this->Cleanup();

    // Setup a cleanup guard.
    SCOPE_GUARD
    (
        if(!m_initialized)
        {
            m_initialized = true;
            this->Cleanup();
        }
    );

    // Add instance to the context.
    if(context.Has<BasicRenderer>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    context.Set(this);

    // Get the resource manager.
    System::ResourceManager* resourceManager = context.Get<System::ResourceManager>();

    if(resourceManager == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing ResourceManager instance.";
        return false;
    }

    // Create a vertex buffer.
    const Vertex vertices[4] =
    {
        // Sprite quad with flipped along y axis texture coordinates.
        { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f) }, // Bottom-Left
        { glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f) }, // Bottom-Right
        { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f) }, // Top-Left
        { glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f) }, // Top-Right
    };

    if(!m_vertexBuffer.Initialize(sizeof(Vertex), Utility::ArraySize(vertices), &vertices[0], GL_STATIC_DRAW))
    {
        Log() << LogInitializeError() << "Couldn't create a vertex buffer.";
        return false;
    }

    // Create an instance buffer.
    if(!m_instanceBuffer.Initialize(sizeof(Sprite::Data), SpriteBatchSize, nullptr, GL_DYNAMIC_DRAW))
    {
        Log() << LogInitializeError() << "Couldn't create an instance buffer.";
        return false;
    }

    // Create a vertex input.
    const VertexAttribute attributes[] =
    {
        { &m_vertexBuffer,   VertexAttributeTypes::Float2   }, // Position
        { &m_vertexBuffer,   VertexAttributeTypes::Float2   }, // Texture
        { &m_instanceBuffer, VertexAttributeTypes::Float4x4 }, // Transform
        { &m_instanceBuffer, VertexAttributeTypes::Float4   }, // Rectangle
        { &m_instanceBuffer, VertexAttributeTypes::Float4   }, // Color
    };

    if(!m_vertexInput.Initialize(Utility::ArraySize(attributes), &attributes[0]))
    {
        Log() << LogInitializeError() << "Couldn't create a vertex input.";
        return false;
    }

    // Create samplers.
    if(!m_nearestSampler.Initialize() || !m_linearSampler.Initialize())
    {
        Log() << LogInitializeError() << "Couldn't create samplers.";
    }

    m_nearestSampler.SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_nearestSampler.SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    m_linearSampler.SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_linearSampler.SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the shader.
    m_shader = resourceManager->Load<Shader>("Data/Shaders/Sprite.glsl");

    if(m_shader == nullptr)
    {
        Log() << LogInitializeError() << "Couldn't load the shader.";
        return false;
    }

    // Make sure we have a valid sprite batch size.
    static_assert(SpriteBatchSize >= 1, "Invalid sprite batch size.");

    // Success!
    return m_initialized = true;
}

void BasicRenderer::Clear(uint32_t flags)
{
    if(!m_initialized)
        return;

    // Clear the frame buffer.
    GLbitfield mask = GL_NONE;

    if(flags & ClearFlags::Color)   mask |= GL_COLOR_BUFFER_BIT;
    if(flags & ClearFlags::Depth)   mask |= GL_DEPTH_BUFFER_BIT;
    if(flags & ClearFlags::Stencil) mask |= GL_STENCIL_BUFFER_BIT;

    glClear(mask);
}

void BasicRenderer::DrawSprite(const Sprite& sprite, const::glm::mat4& transform)
{
    if(!m_initialized)
        return;

    // Bind the vertex input.
    glBindVertexArray(m_vertexInput.GetHandle());

    SCOPE_GUARD
    (
        glBindVertexArray(0);
    );

    // Bind shader program.
    glUseProgram(m_shader->GetHandle());

    SCOPE_GUARD
    (
        glUseProgram(0);
    );

    glUniformMatrix4fv(m_shader->GetUniform("viewTransform"), 1, GL_FALSE, glm::value_ptr(transform));

    // Setup transparency.
    if(sprite.info.transparent)
    {
        // Enable alpha blending.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Disable depth writing.
        glDepthMask(GL_FALSE);
    }

    SCOPE_GUARD
    (
        if(sprite.info.transparent)
        {
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
        }
    );

    // Setup texture.
    if(sprite.info.texture != nullptr)
    {
        // Calculate inversed texture size.
        glm::vec2 textureInvSize;
        textureInvSize.x = 1.0f / sprite.info.texture->GetWidth();
        textureInvSize.y = 1.0f / sprite.info.texture->GetHeight();

        glUniform2fv(m_shader->GetUniform("textureSizeInv"), 1, glm::value_ptr(textureInvSize));

        // Bind texture unit.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sprite.info.texture->GetHandle());

        // Bind texture sampler.
        if(sprite.info.filter)
        {
            glBindSampler(0, m_linearSampler.GetHandle());
        }
        else
        {
            glBindSampler(0, m_nearestSampler.GetHandle());
        }
    }

    SCOPE_GUARD
    (
        if(sprite.info.texture != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    );

    glUniform1i(m_shader->GetUniform("textureDiffuse"), 0);

    // Update the instance buffer with sprite data.
    m_instanceBuffer.Update(&sprite.data, 1);

    // Draw instanced sprite.
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
}

void BasicRenderer::DrawSprites(const SpriteInfoList& spriteInfo, const SpriteDataList& spriteData, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    // Make sure both lists have the same size.
    if(spriteInfo.size() != spriteData.size())
        return;

    const int spriteCount = spriteInfo.size();

    // Bind the vertex input.
    glBindVertexArray(m_vertexInput.GetHandle());

    SCOPE_GUARD
    (
        glBindVertexArray(0);
    );

    // Bind shader program.
    glUseProgram(m_shader->GetHandle());

    SCOPE_GUARD
    (
        glUseProgram(0);
    );

    glUniformMatrix4fv(m_shader->GetUniform("viewTransform"), 1, GL_FALSE, glm::value_ptr(transform));

    // Current transparency state.
    bool currentTransparent = false;

    SCOPE_GUARD
    (
        if(currentTransparent)
        {
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
        }
    );

    // Current texture state.
    const Texture* currentTexture = nullptr;

    SCOPE_GUARD
    (
        if(currentTexture != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    );

    glUniform1i(m_shader->GetUniform("textureDiffuse"), 0);

    // Render sprites.
    int spritesDrawn = 0;

    while(spritesDrawn != spriteCount)
    {
        // Get the first sprite info that will represent current batch.
        const Sprite::Info& info = spriteInfo[spritesDrawn];

        // Add similar sprites to the current batch.
        int spritesBatched = 1;

        while(true)
        {
            // Check if we reached the maximum batch size.
            if(spritesBatched == SpriteBatchSize)
                break;

            // Get the index of the next sprite.
            int spriteNext = spritesDrawn + spritesBatched;

            // Check if we reached the end of the sprite list.
            if(spriteNext >= spriteCount)
                break;

            // Check if the next sprite can be batched.
            if(info != spriteInfo[spriteNext])
                break;

            // Add sprite to batch.
            ++spritesBatched;
        }

        // Update the instance buffer with sprite data.
        m_instanceBuffer.Update(&spriteData[spritesDrawn], spritesBatched);

        // Set transparency state.
        if(currentTransparent != info.transparent)
        {
            if(info.transparent)
            {
                // Enable alpha blending.
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                // Disable depth writing.
                glDepthMask(GL_FALSE);
            }
            else
            {
                // Disable alpha blending.
                glDisable(GL_BLEND);

                // Enable depth writing.
                glDepthMask(GL_TRUE);
            }

            currentTransparent = info.transparent;
        }

        // Set texture state.
        if(currentTexture != info.texture)
        {
            // Set texture uniform.
            if(info.texture != nullptr)
            {
                // Calculate inversed texture size.
                glm::vec2 textureInvSize;
                textureInvSize.x = 1.0f / info.texture->GetWidth();
                textureInvSize.y = 1.0f / info.texture->GetHeight();

                glUniform2fv(m_shader->GetUniform("textureSizeInv"), 1, glm::value_ptr(textureInvSize));

                // Bind texture unit.
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, info.texture->GetHandle());
                
                // Bind texture sampler.
                if(info.filter)
                {
                    glBindSampler(0, m_linearSampler.GetHandle());
                }
                else
                {
                    glBindSampler(0, m_nearestSampler.GetHandle());
                }
            }
            else
            {
                // Disable texture unit.
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            currentTexture = info.texture;
        }

        // Draw instanced sprite batch.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, spritesBatched);

        // Update the counter of drawn sprites.
        spritesDrawn += spritesBatched;
    }
}

void BasicRenderer::SetClearColor(const glm::vec4& color)
{
    if(!m_initialized)
        return;

    glClearColor(color.r, color.g, color.b, color.a);
}

void BasicRenderer::SetClearDepth(float depth)
{
    if(!m_initialized)
        return;

    glClearDepth(depth);
}

void BasicRenderer::SetClearStencil(int stencil)
{
    if(!m_initialized)
        return;

    glClearStencil(stencil);
}
