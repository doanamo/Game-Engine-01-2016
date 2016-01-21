#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

//
// Identity System
//
//  Assigns readable names to entities that they can be refered by.
//
//  Example usage:
//      EntityHandle entity = identitySystem.Lookup("Player");
//      identitySystem.SetEntityName(entity, "Player 1");
//
//  Todo:
//  - Add way to assign other generic attributes to entities (ex. groups).
//    Possibly using templates to be able to assign values associated with any type.
//

namespace Game
{
    // Identity system class.
    class IdentitySystem
    {
    private:
        // Comparison functor.
        template<typename Type>
        struct CompareDereferenced
        {
            bool operator()(const Type* left, const Type* right) const
            {
                return *left < *right;
            }
        };

        // Type definitions.
        typedef std::pair<EntityHandle, std::string> EntityNamePair;
        typedef std::deque<EntityNamePair> EntityNameRegistry;

        typedef std::map<const EntityHandle*, std::size_t, CompareDereferenced<EntityHandle>> EntityLookupList;
        typedef std::map<const std::string*, std::size_t, CompareDereferenced<std::string>> NameLookupList;

    public:
        IdentitySystem();
        ~IdentitySystem();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the identity system instance.
        bool Initialize(Context& context);

        // Sets entity name.
        bool SetEntityName(EntityHandle entity, std::string name);

        // Gets entity name.
        std::string GetEntityName(EntityHandle entity) const;

        // Lookups an entity by name.
        EntityHandle Lookup(std::string name) const;

    private:
        // Removes an element by index.
        void RemoveElement(std::size_t index);

        // Called when an entity gets destroyed.
        void OnEntityDestroyed(EntityHandle handle);

    private:
        // Registry of named entities.
        EntityNameRegistry m_entities;
        
        // Lookup indices.
        EntityLookupList m_entityLookup;
        NameLookupList m_nameLookup;

        // Event receivers.
        Receiver<void(EntityHandle)> m_entityDestroyed;

        // Initialization state.
        bool m_initialized;
    };
}
