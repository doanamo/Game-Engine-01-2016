#include "Precompiled.hpp"
#include "RenderSystem.hpp"
#include "ComponentSystem.hpp"
#include "Components/Transform.hpp"
#include "Components/Render.hpp"
#include "System/Window.hpp"
#include "Context.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the render system! "

    // Global render scale.
    const glm::vec3 RenderScale(1.0f / 16.0f, 1.0f / 16.0f, 1.0f);
}

RenderSystem::RenderSystem() :
    m_window(nullptr),
    m_basicRenderer(nullptr),
    m_componentSystem(nullptr),
    m_initialized(false)
{
}

RenderSystem::~RenderSystem()
{
    this->Cleanup();
}

void RenderSystem::Cleanup()
{
    if(!m_initialized)
        return;

    // Reset context references.
    m_window = nullptr;
    m_basicRenderer = nullptr;
    m_componentSystem = nullptr;

    // Reset screen space transform.
    m_screenSpace.Cleanup();

    // Cleanup sprite lists.
    Utility::ClearContainer(m_spriteInfo);
    Utility::ClearContainer(m_spriteData);
    Utility::ClearContainer(m_spriteSort);

    // Reset initialization state.
    m_initialized = false;
}

bool RenderSystem::Initialize(Context& context)
{
    Assert(context.window != nullptr);
    Assert(context.basicRenderer != nullptr);
    Assert(context.componentSystem != nullptr);
    Assert(context.renderSystem == nullptr);

    // Cleanup this instance.
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

    // Get required context instances.
    m_window = context.window;
    m_basicRenderer = context.basicRenderer;
    m_componentSystem = context.componentSystem;

    // Set screen space target size.
    m_screenSpace.SetTargetSize(10.0f, 10.0f);

    // Allocate initial sprite list memory.
    const int SpriteListSize = 128;
    m_spriteInfo.reserve(SpriteListSize);
    m_spriteData.reserve(SpriteListSize);
    m_spriteSort.reserve(SpriteListSize);

    // Set context instance.
    context.renderSystem = this;

    // Success!
    return m_initialized = true;
}

void RenderSystem::Draw()
{
    if(!m_initialized)
        return;

    // Get window size.
    int windowWidth = m_window->GetWidth();
    int windowHeight = m_window->GetHeight();

    // Set viewport size.
    glViewport(0, 0, windowWidth, windowHeight);

    // Set screen space source size.
    m_screenSpace.SetSourceSize(windowWidth, windowHeight);

    // Calculate camera view.
    glm::mat4 view = glm::translate(glm::mat4(1.0f), -glm::vec3(m_screenSpace.GetOffset(), 0.0f));
    glm::mat4 transform = m_screenSpace.GetTransform() * view;

    // Clear the backbuffer.
    m_basicRenderer->SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_basicRenderer->SetClearDepth(1.0f);
    m_basicRenderer->Clear();

    // Iterate over all render components.
    auto componentsBegin = m_componentSystem->Begin<Components::Render>();
    auto componentsEnd = m_componentSystem->End<Components::Render>();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        // Get entity components.
        Components::Render* render = &it->second;
        Assert(render != nullptr);

        Components::Transform* transform = render->GetTransform();
        Assert(transform != nullptr);

        // Add sprite to render the list.
        Graphics::BasicRenderer::Sprite::Info info;
        info.texture = render->GetTexture().get();
        info.transparent = render->IsTransparent();
        info.filter = false;

        Graphics::BasicRenderer::Sprite::Data data;
        data.transform = glm::translate(data.transform, glm::vec3(transform->GetPosition(), 0.0f));
//      data.transform = glm::rotate(data.transform, transform->GetRotation(), glm::vec3(0.0f, 0.0f, -1.0f));
        data.transform = glm::scale(data.transform, glm::vec3(transform->GetScale(), 1.0f) * RenderScale);
        data.transform = glm::translate(data.transform, glm::vec3(render->GetOffset(), 0.0f));
        data.rectangle = render->GetRectangle();
        data.color = render->CalculateColor();

        m_spriteInfo.push_back(info);
        m_spriteData.push_back(data);
    }

    // Define sorting function.
    auto SpriteSort = [&](const int& a, const int& b)
    {
        // Get sprite info and data.
        const auto& spriteInfoA = m_spriteInfo[a];
        const auto& spriteDataA = m_spriteData[a];

        const auto& spriteInfoB = m_spriteInfo[b];
        const auto& spriteDataB = m_spriteData[b];

        // Sort by transparency (opaque first, transparent second).
        if(spriteInfoA.transparent < spriteInfoB.transparent)
            return true;

        if(spriteInfoA.transparent == spriteInfoB.transparent)
        {
            if(spriteInfoA.transparent)
            {
                // Sort transparent by depth (back to front).
                if(spriteDataA.transform[3][2] < spriteDataB.transform[3][2])
                    return true;

                if(spriteDataA.transform[3][2] == spriteDataB.transform[3][2])
                {
                    // Sort by the y position.
                    if(spriteDataA.transform[3][1] > spriteDataB.transform[3][1])
                        return true;

                    if(spriteDataA.transform[3][1] == spriteDataB.transform[3][1])
                    {
                        // Sort by texture.
                        if(spriteInfoA.texture < spriteInfoB.texture)
                            return true;
                    }
                }
            }
            else
            {
                // Sort opaque by depth (front to back).
                if(spriteDataA.transform[3][2] > spriteDataB.transform[3][2])
                    return true;

                if(spriteDataA.transform[3][2] == spriteDataB.transform[3][2])
                {
                    // Sort by texture.
                    if(spriteInfoA.texture < spriteInfoB.texture)
                        return true;
                }
            }
        }

        return false;
    };

    // Create sort permutation.
    Assert(m_spriteInfo.size() == m_spriteData.size());

    m_spriteSort.resize(m_spriteInfo.size());
    std::iota(m_spriteSort.begin(), m_spriteSort.end(), 0);
    std::sort(m_spriteSort.begin(), m_spriteSort.end(), SpriteSort);

    // Sort sprite lists.
    Utility::Reorder(m_spriteInfo, m_spriteSort);
    Utility::Reorder(m_spriteData, m_spriteSort);

    // Draw sprites.
    m_basicRenderer->DrawSprites(m_spriteInfo, m_spriteData, transform);

    // Clear the sprite list.
    m_spriteInfo.clear();
    m_spriteData.clear();
}
