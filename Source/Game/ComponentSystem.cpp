#include "Precompiled.hpp"
#include "ComponentSystem.hpp"
#include "EntitySystem.hpp"
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
    if(m_initialized)
        this->Cleanup();
}

void ComponentSystem::Cleanup()
{
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
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Check if the instance already exists.
    if(context.Has<ComponentSystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    // Get required instances.
    EntitySystem* entitySystem = context.Get<EntitySystem>();

    if(entitySystem == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing EntitySystem instance.";
        return false;
    }

    // Subscribe event receivers.
    entitySystem->events.entityFinalize.Subscribe(m_entityFinalize);
    entitySystem->events.entityDestroyed.Subscribe(m_entityDestroyed);

    // Add instance to the context.
    context.Set(this);

    // Save context reference.
    m_context = &context;

    // Success!
    return m_initialized = true;
}

bool ComponentSystem::OnEntityFinalize(EntityHandle handle)
{
    assert(m_initialized);
    assert(m_context != nullptr);

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
    assert(m_initialized);

    // Remove entity components from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;
        pool->Remove(handle);
    }
}
