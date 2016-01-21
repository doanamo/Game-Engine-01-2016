#include "Precompiled.hpp"
#include "EntitySystem.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the entity system! "

    // Constant variables.
    const int MaximumIdentifier   = std::numeric_limits<int>::max();
    const int InvalidIdentifier   = 0;
    const int InvalidNextFree     = -1;
    const int InvalidQueueElement = -1;
}

EntitySystem::EntitySystem() :
    events(m_dispatchers),
    m_entityCount(0),
    m_freeListDequeue(InvalidQueueElement),
    m_freeListEnqueue(InvalidQueueElement),
    m_freeListIsEmpty(true),
    m_initialized(false)
{
}

EntitySystem::Events::Events(EventDispatchers& dispatchers) :
    entityFinalize(dispatchers.entityFinalize),
    entityCreated(dispatchers.entityCreated),
    entityDestroyed(dispatchers.entityDestroyed)
{
}

EntitySystem::~EntitySystem()
{
    if(m_initialized)
        this->Cleanup();
}

void EntitySystem::Cleanup()
{
    // Destroy all remaining entities.
    this->DestroyAllEntities();
    this->ProcessCommands();

    // Cleanup event dispatchers.
    m_dispatchers.entityFinalize.Cleanup();
    m_dispatchers.entityCreated.Cleanup();
    m_dispatchers.entityDestroyed.Cleanup();

    // Clear the command list.
    Utility::ClearContainer(m_commands);

    // Clear the handle list.
    Utility::ClearContainer(m_handles);

    // Reset the entity counter.
    m_entityCount = 0;

    // Reset the list of free handles.
    m_freeListDequeue = InvalidQueueElement;
    m_freeListEnqueue = InvalidQueueElement;
    m_freeListIsEmpty = true;

    // Reset initialization state.
    m_initialized = false;
}

bool EntitySystem::Initialize(Context& context)
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
    if(context.Has<EntitySystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    // Add instance to the context.
    context.Set(this);

    // Success!
    return m_initialized = true;
}

EntityHandle EntitySystem::CreateEntity()
{
    if(!m_initialized)
        return EntityHandle();

    // Check if we reached the numerical limits.
    assert(m_handles.size() != MaximumIdentifier);

    // Create a new handle if the free list queue is empty.
    if(m_freeListIsEmpty)
    {
        // Create an entity handle.
        EntityHandle handle;
        handle.identifier = m_handles.size() + 1;
        handle.version = 0;

        // Create a handle entry.
        HandleEntry entry;
        entry.handle = handle;
        entry.nextFree = InvalidNextFree;
        entry.flags = HandleFlags::Free;

        m_handles.push_back(entry);

        // Add new handle entry to the free list queue.
        int handleIndex = m_handles.size() - 1;

        m_freeListDequeue = handleIndex;
        m_freeListEnqueue = handleIndex;
        m_freeListIsEmpty = false;
    }

    // Retrieve an unused handle from the free list.
    int handleIndex = m_freeListDequeue;
    HandleEntry& handleEntry = m_handles[handleIndex];

    // Clear next free handle index.
    handleEntry.nextFree = InvalidNextFree;

    // Update the free list queue.
    if(m_freeListDequeue == m_freeListEnqueue)
    {
        // If there was only one element in the queue,
        // set the free list queue state to empty.
        m_freeListDequeue = InvalidQueueElement;
        m_freeListEnqueue = InvalidQueueElement;
        m_freeListIsEmpty = true;
    }
    else
    {
        // If there were more than a single element in the queue,
        // set the beginning of the queue to the next free element.
        m_freeListDequeue = handleEntry.nextFree;
    }

    // Mark handle as valid.
    handleEntry.flags |= HandleFlags::Valid;

    // Add a create entity command.
    EntityCommand command;
    command.type = EntityCommands::Create;
    command.handle = handleEntry.handle;

    m_commands.push(command);

    // Return the handle, which is still inactive
    // until the next ProcessCommands() call.
    return handleEntry.handle;
}

void EntitySystem::DestroyEntity(const EntityHandle& entity)
{
    if(!m_initialized)
        return;

    // Check if the handle is valid.
    if(!IsHandleValid(entity))
        return;

    // Locate the handle entry.
    int handleIndex = entity.identifier - 1;
    HandleEntry& handleEntry = m_handles[handleIndex];

    // Set the handle destroy flag.
    handleEntry.flags |= HandleFlags::Destroy;

    // Add a destroy entity command.
    EntityCommand command;
    command.type = EntityCommands::Destroy;
    command.handle = handleEntry.handle;

    m_commands.push(command);
}

void EntitySystem::DestroyAllEntities()
{
    if(!m_initialized)
        return;

    // Process entity commands.
    ProcessCommands();

    // Check if there are any entities to destroy.
    if(m_handles.empty())
        return;

    // Inform about entities soon to be destroyed.
    for(auto it = m_handles.begin(); it != m_handles.end(); ++it)
    {
        HandleEntry& handleEntry = *it;

        if(handleEntry.flags & HandleFlags::Valid)
        {
            // Send event about soon to be destroyed entity.
            m_dispatchers.entityDestroyed(handleEntry.handle);

            // Set the handle free flags.
            handleEntry.flags = HandleFlags::Free;

            // Increment the handle version to invalidate it.
            handleEntry.handle.version += 1;
        }
    }

    // Chain handles to form a free list.
    for(unsigned int i = 0; i < m_handles.size(); ++i)
    {
        HandleEntry& handleEntry = m_handles[i];
        handleEntry.nextFree = i + 1;
    }

    // Close the free list queue chain at the end.
    int lastHandleIndex = m_handles.size() - 1;
    m_handles[lastHandleIndex].nextFree = InvalidNextFree;

    // Set the free list variables.
    m_freeListDequeue = 0;
    m_freeListEnqueue = lastHandleIndex;
    m_freeListIsEmpty = false;
}

void EntitySystem::ProcessCommands()
{
    if(!m_initialized)
        return;

    // Process entity commands.
    while(!m_commands.empty())
    {
        // Get the command from the queue.
        EntityCommand& command = m_commands.front();

        // Process entity command.
        switch(command.type)
        {
        case EntityCommands::Create:
            {
                // Locate the handle entry.
                int handleIndex = command.handle.identifier - 1;
                HandleEntry& handleEntry = m_handles[handleIndex];

                // Make sure handles match.
                assert(command.handle == handleEntry.handle);

                // Inform that we want this entity finalized.
                if(!m_dispatchers.entityFinalize(handleEntry.handle))
                {
                    // Destroy the entity.
                    this->DestroyEntity(handleEntry.handle);
                    break;
                }

                // Mark handle as active.
                assert(!(handleEntry.flags & HandleFlags::Active));

                handleEntry.flags |= HandleFlags::Active;

                // Increment the counter of active entities.
                m_entityCount += 1;

                // Send event about created entity.
                m_dispatchers.entityCreated(handleEntry.handle);
            }
            break;

        case EntityCommands::Destroy:
            {
                // Locate the handle entry.
                int handleIndex = command.handle.identifier - 1;
                HandleEntry& handleEntry = m_handles[handleIndex];

                // Check if handles match.
                if(command.handle != handleEntry.handle)
                {
                    // Trying to destroy an entity twice.
                    assert(false);
                    continue;
                }

                // Send event about soon to be destroyed entity.
                m_dispatchers.entityDestroyed(handleEntry.handle);

                // Decrement the counter of active entities.
                m_entityCount -= 1;

                // Free entity handle.
                assert(handleEntry.flags & HandleFlags::Destroy);

                this->FreeHandle(handleIndex, handleEntry);
            }
            break;
        }

        // Remove command from the queue.
        m_commands.pop();
    }
}

void EntitySystem::FreeHandle(int handleIndex, HandleEntry& handleEntry)
{
    if(!m_initialized)
        return;

    // Make sure we got the matching index.
    assert(&m_handles[handleIndex] == &handleEntry);

    // Mark handle as free.
    assert(handleEntry.flags & HandleFlags::Valid);
    assert(!(handleEntry.flags & HandleFlags::Free));

    handleEntry.flags = HandleFlags::Free;

    // Increment the handle version to invalidate it.
    handleEntry.handle.version += 1;

    // Add the handle entry to the free list queue.
    if(m_freeListIsEmpty)
    {
        // If there are no elements in the queue,
        // set the element as the only one in the queue.
        m_freeListDequeue = handleIndex;
        m_freeListEnqueue = handleIndex;
        m_freeListIsEmpty = false;
    }
    else
    {
        assert(m_handles[m_freeListEnqueue].nextFree == InvalidNextFree);

        // If there are already other elements in the queue,
        // add the element to the end of the queue chain.
        m_handles[m_freeListEnqueue].nextFree = handleIndex;
        m_freeListEnqueue = handleIndex;
    }
}

bool EntitySystem::IsHandleValid(const EntityHandle& entity) const
{
    if(!m_initialized)
        return false;

    // Check if the handle identifier is valid.
    if(entity.identifier <= InvalidIdentifier)
        return false;

    if(entity.identifier > (int)m_handles.size())
        return false;

    // Locate the handle entry.
    int handleIndex = entity.identifier - 1;
    const HandleEntry& handleEntry = m_handles[handleIndex];

    // Check if handle is valid.
    if(!(handleEntry.flags & HandleFlags::Valid))
        return false;

    // Check if handle is scheduled to be destroyed.
    if(handleEntry.flags & HandleFlags::Destroy)
        return false;

    // Check if handle versions match.
    if(handleEntry.handle.version != entity.version)
        return false;

    return true;
}
