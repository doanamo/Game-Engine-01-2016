#include "Precompiled.hpp"
#include "IdentitySystem.hpp"
#include "EntitySystem.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the identity system! "

    // Invalid name constant.
    const char* InvalidName = "";
}

IdentitySystem::IdentitySystem() :
    m_initialized(false)
{
    // Bind event receivers.
    m_entityDestroyed.Bind<IdentitySystem, &IdentitySystem::OnEntityDestroyed>(this);
}

IdentitySystem::~IdentitySystem()
{
    this->Cleanup();
}

void IdentitySystem::Cleanup()
{
    if(!m_initialized)
        return;

    // Clear entity registry.
    Utility::ClearContainer(m_entities);

    // Clear lookup tables.
    Utility::ClearContainer(m_entityLookup);
    Utility::ClearContainer(m_nameLookup);

    // Unsubscribe receivers.
    m_entityDestroyed.Unsubscribe();

    // Reset initialization state.
    m_initialized = false;
}

bool IdentitySystem::Initialize(Context& context)
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

    // Check if the instance already exists.
    if(context.Has<IdentitySystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    // Get the entity system.
    EntitySystem* entitySystem = context.Get<EntitySystem>();

    if(entitySystem == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing EntitySystem instance.";
        return false;
    }

    // Subscribe receivers.
    entitySystem->events.entityDestroyed.Subscribe(m_entityDestroyed);

    // Add instance to the context.
    context.Set(this);

    // Success!
    return m_initialized = true;
}

bool IdentitySystem::SetEntityName(EntityHandle entity, std::string name)
{
    if(!m_initialized)
        return false;

    // Find out if we already have this entity named.
    // Pointer that's passed below will be dereferenced.
    auto it = m_entityLookup.find(&entity);

    if(it != m_entityLookup.end())
    {
        std::size_t index = it->second;

        // If name is not empty, rename entity.
        // If name is empty, remove entity from registry.
        if(!name.empty())
        {
            // Replace entity name.
            m_entities[index].second = name;
        }
        else
        {
            // Remove named entity.
            this->RemoveElement(index);
        }
    }
    else
    {
        // Create new element in the registry.
        m_entities.emplace_back(entity, name);

        // Create lookup references.
        m_entityLookup.emplace(&m_entities.back().first, m_entities.size() - 1);
        m_nameLookup.emplace(&m_entities.back().second, m_entities.size() - 1);
    }

    return true;
}

std::string IdentitySystem::GetEntityName(EntityHandle entity) const
{
    if(!m_initialized)
        return InvalidName;

    // Find the entity.
    // Pointer that's passed below will be dereferenced.
    auto it = m_entityLookup.find(&entity);

    if(it != m_entityLookup.end())
    {
        // Return entity name.
        std::size_t index = it->second;
        return m_entities[index].second;
    }
    else
    {
        // Entity is annonymous.
        return InvalidName;
    }
}

EntityHandle IdentitySystem::Lookup(std::string name) const
{
    if(!m_initialized)
        return EntityHandle();

    // Find the name.
    // Pointer that's passed below will be dereferenced.
    auto it = m_nameLookup.find(&name);

    if(it != m_nameLookup.end())
    {
        // Return named entity.
        std::size_t index = it->second;
        return m_entities[index].first;
    }
    else
    {
        // Return an invalid entity.
        return EntityHandle();
    }
}

void IdentitySystem::RemoveElement(std::size_t index)
{
    assert(m_initialized);

    // Move registry element to the end of the queue before erasing it to 
    // avoid invalidating pointers/indices referencing the container.
    m_entityLookup[&m_entities.back().first] = index;
    m_nameLookup[&m_entities.back().second] = index;

    std::swap(m_entities[index], m_entities.back());

    // Remove the element we moved that's at the back now.
    m_entityLookup.erase(&m_entities.back().first);
    m_nameLookup.erase(&m_entities.back().second);

    m_entities.pop_back();
}

void IdentitySystem::OnEntityDestroyed(EntityHandle handle)
{
    assert(m_initialized);

    // Remove entity if it was registered.
    // Pointer that's passed below will be dereferenced.
    auto it = m_entityLookup.find(&handle);
    
    if(it != m_entityLookup.end())
    {
        std::size_t index = it->second;
        this->RemoveElement(index);
    }
}
