#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

//
// Entity System
//
//  Manages unique identities of each existing entity. Gives means to identify
//  different entities and takes care of their creation/destruction.
//
//  Example usage:
//      Game::EntitySystem entitySystem;
//      entitySystem.Initialize(context);
//      
//      EntityHandle entity = entitySystem.CreateEntity();
//      /*
//          Add components here!
//          Entity remains inactive until
//          the next ProcessCommands() call.
//      */
//      entitySystem.ProcessCommands();
//      
//      entitySystem.DestroyEntity(entity);
//      /*
//          Entity remains active until
//          the next ProcessCommands() call.
//      */
//      entitySystem.ProcessCommands();
//

namespace Game
{
    // Entity system class.
    class EntitySystem
    {
    public:
        // Handle flags.
        struct HandleFlags
        {
            enum Type
            {
                // Entity handle has been allocated but is not being used.
                None = 0,

                // Entity handle has been created and is valid.
                Valid = 1 << 0,

                // Entity handle has been created and is active.
                Active = 1 << 1,

                // Entity handle has been scheduled to be destroyed.
                Destroy = 1 << 2,
            };

            static const uint32_t Free = None;
        };

        // Handle entry structure.
        struct HandleEntry
        {
            EntityHandle handle;
            int          nextFree;
            uint32_t     flags;
        };

        // Entity command types.
        struct EntityCommands
        {
            enum Type
            {
                Invalid,

                Create,
                Destroy,
            };
        };

        // Entity command structure.
        struct EntityCommand
        {
            EntityCommands::Type type;
            EntityHandle handle;
        };

    private:
        // Type declarations.
        typedef std::vector<HandleEntry>  HandleList;
        typedef std::queue<EntityCommand> CommandList;

    public:
        EntitySystem();
        ~EntitySystem();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the entity system instance.
        bool Initialize(Context& context);

        // Creates an entity.
        EntityHandle CreateEntity();

        // Destroys an entity.
        void DestroyEntity(const EntityHandle& entity);

        // Destroys all entities.
        void DestroyAllEntities();

        // Process entity commands.
        void ProcessCommands();

        // Checks if an entity handle is valid.
        bool IsHandleValid(const EntityHandle& entity) const;

        // Returns the number of active entities.
        unsigned int GetEntityCount() const
        {
            return m_entityCount;
        }

    private:
        // Frees an entity handle.
        void FreeHandle(int handleIndex, HandleEntry& handleEntry);

    public:
        // Public event dispatchers.
        struct EventDispatchers;

        struct Events
        {
            Events(EventDispatchers& dispatchers);

            DispatcherBase<bool(EntityHandle)>& entityFinalize;
            DispatcherBase<void(EntityHandle)>& entityCreated;
            DispatcherBase<void(EntityHandle)>& entityDestroyed;
        } events;

        // Private event dispatchers.
        struct EventDispatchers
        {
            Dispatcher<bool(EntityHandle), CollectWhileTrue<bool>> entityFinalize;
            Dispatcher<void(EntityHandle)> entityCreated;
            Dispatcher<void(EntityHandle)> entityDestroyed;
        };

    private:
        // List of commands.
        CommandList m_commands;

        // List of entity handles.
        HandleList m_handles;

        // Number of active entities.
        unsigned int m_entityCount;

        // List of free handles.
        int  m_freeListDequeue;
        int  m_freeListEnqueue;
        bool m_freeListIsEmpty;

        // Event dispatchers.
        EventDispatchers m_dispatchers;

        // Initialization state.
        bool m_initialized;
    };
}
