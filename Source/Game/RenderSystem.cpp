#include "Precompiled.hpp"
#include "RenderSystem.hpp"
#include "System/Window.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the render system! "
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

    // Add instance to the context.
    context.Set(this);

    // Success!
    return m_initialized = true;
}

void RenderSystem::Draw()
{
    if(!m_initialized)
        return;

    m_basicRenderer->SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_basicRenderer->SetClearDepth(1.0f);
    m_basicRenderer->Clear();
}
