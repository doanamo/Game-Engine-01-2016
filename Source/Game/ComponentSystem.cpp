#include "Precompiled.hpp"
#include "ComponentSystem.hpp"
#include "EntitySystem.hpp"
#include "Context.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the component system! "
}

ComponentSystem::ComponentSystem() :
    m_context(nullptr),
    m_initialized(false)
{
    // Bind event receivers.
    m_entityFinalize.Bind<ComponentSystem, &ComponentSystem::OnEntityFinalize>(this);
    m_entityDestroyed.Bind<ComponentSystem, &ComponentSystem::OnEntityDestroyed>(this);
}

ComponentSystem::~ComponentSystem()
{
    this->Cleanup();
}

void ComponentSystem::Cleanup()
{
    if(!m_initialized)
        return;

    // Clear all component pools.
    Utility::ClearContainer(m_pools);

    // Unsubscribe event signals.
    m_entityFinalize.Unsubscribe();
    m_entityDestroyed.Unsubscribe();

    // Reset context reference.
    m_context = nullptr;

    // Reset initialization state.
    m_initialized = false;
}

bool ComponentSystem::Initialize(Context& context)
{
    Assert(context.entitySystem != nullptr);
    Assert(context.componentSystem == nullptr);

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

    // Subscribe event receivers.
    context.entitySystem->events.entityFinalize.Subscribe(m_entityFinalize);
    context.entitySystem->events.entityDestroyed.Subscribe(m_entityDestroyed);

    // Set context instance.
    context.componentSystem = this;

    // Save context reference.
    m_context = &context;

    // Success!
    return m_initialized = true;
}

bool ComponentSystem::OnEntityFinalize(EntityHandle handle)
{
    Assert(m_initialized);
    Assert(m_context != nullptr);

    // Finalize entity components from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;

        if(!pool->Finalize(handle, *m_context))
            return false;
    }

    return true;
}

void ComponentSystem::OnEntityDestroyed(EntityHandle handle)
{
    Assert(m_initialized);

    // Remove entity components from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;
        pool->Remove(handle);
    }
}
