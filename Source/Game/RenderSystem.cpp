#include "Precompiled.hpp"
#include "RenderSystem.hpp"
#include "System/Window.hpp"
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
    m_initialized(false)
{
}

RenderSystem::~RenderSystem()
{
    Cleanup();
}

void RenderSystem::Cleanup()
{
    // Reset context references.
    m_window = nullptr;
    m_basicRenderer = nullptr;

    // Reset screen space transform.
    m_screenSpace.Cleanup();

    // Reset initialization state.
    m_initialized = false;
}

bool RenderSystem::Initialize(Context& context)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Check if the instance already exists.
    if(context.Has<RenderSystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    // Get required instances.
    m_window = context.Get<System::Window>();

    if(m_window == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing Window instance.";
        return false;
    }

    m_basicRenderer = context.Get<Graphics::BasicRenderer>();

    if(m_basicRenderer == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing BasicRenderer instance.";
        return false;
    }

    // Set screen space target size.
    m_screenSpace.SetTargetSize(10.0f, 10.0f);

    // Add instance to the context.
    context.Set(this);

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
}
