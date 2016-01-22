#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the script system! "
}

ScriptSystem::ScriptSystem() :
    m_initialized(false)
{
}

ScriptSystem::~ScriptSystem()
{
    if(m_initialized)
        this->Cleanup();
}

void ScriptSystem::Cleanup()
{
    // Reset initialization state.
    m_initialized = false;
}

bool ScriptSystem::Initialize(Context& context)
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
    if(context.Has<ScriptSystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    // Add instance to the context.
    context.Set(this);

    // Success!
    return m_initialized = true;
}

void ScriptSystem::Update(float timeDelta)
{
    if(!m_initialized)
        return;
}
