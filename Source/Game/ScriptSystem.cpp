#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "EntitySystem.hpp"
#include "ComponentSystem.hpp"
#include "Components/Script.hpp"
#include "Context.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the script system! "
}

ScriptSystem::ScriptSystem() :
    m_entitySystem(nullptr),
    m_componentSystem(nullptr),
    m_initialized(false)
{
}

ScriptSystem::~ScriptSystem()
{
    this->Cleanup();
}

void ScriptSystem::Cleanup()
{
    if(!m_initialized)
        return;

    // Reset context references.
    m_entitySystem = nullptr;
    m_componentSystem = nullptr;

    // Reset Lua state.
    m_state = nullptr;

    // Reset initialization state.
    m_initialized = false;
}

bool ScriptSystem::Initialize(Context& context)
{
    Assert(context.entitySystem != nullptr);
    Assert(context.componentSystem != nullptr);
    Assert(context.scriptSystem == nullptr);

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

    // Get required context references.
    m_entitySystem = context.entitySystem;
    m_componentSystem = context.componentSystem;

    // Initialize Lua state.
    m_state = std::make_shared<Lua::State>();

    if(!m_state->Initialize())
    {
        Log() << LogInitializeError() << "Couldn't initialize Lua state.";
        return false;
    }

    // Register the context for use in Lua.
    if(!this->RegisterContext(context))
    {
        Log() << LogInitializeError() << "Couldn't register the context.";
        return false;
    }

    // Set context instance.
    context.scriptSystem = this;

    // Success!
    return m_initialized = true;
}

void ScriptSystem::Update(float timeDelta)
{
    if(!m_initialized)
        return;

    // Collect script garbage.
    m_state->CollectGarbage(0.002f);

    // Update all script components.
    auto componentsBegin = m_componentSystem->Begin<Components::Script>();
    auto componentsEnd = m_componentSystem->End<Components::Script>();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        const EntityHandle& entity = it->first;
        Components::Script& script = it->second;

        // Check if entity is active.
        if(!m_entitySystem->IsHandleValid(it->first))
            continue;

        // Update script component.
        script.Call("Update", entity, timeDelta);
    }
}

std::shared_ptr<Lua::State> ScriptSystem::GetState()
{
    if(!m_initialized)
        return nullptr;

    return m_state;
}
