#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"
#include "ComponentPool.hpp"

//
// Component System
//
//  Manages component instances and ties them to entities.
//
//  Adding components to entities.
//      EntityHandle entity = entitySystem.CreateEntity();
//      auto transform = componentSystem.Create<Game::Components::Transform>(entity);
//      auto Render = componentSystem.Create<Game::Components::Render>(entity);
//      entitySystem.ProcessCommands();
//
//  Declaring a component type:
//      class Class : public Game::Component
//      {
//          /* ... */
//      };
//
//  Iterate over all components of a type:
//      auto componentsBegin = m_componentSystem->Begin<Components::Class>();
//      auto componentsEnd = m_componentSystem->End<Components::Class>();
//      
//      for(auto it = componentsBegin; it != componentsEnd; ++it)
//      {
//          const EntityHandle& entity = it->first;
//          Components::Class& component = it->second;
//          
//          /* ... */
//      }
//

namespace Game
{
    // Component system class.
    class ComponentSystem
    {
    public:
        // Type declarations.
        typedef std::unique_ptr<ComponentPoolInterface>               ComponentPoolPtr;
        typedef std::unordered_map<std::type_index, ComponentPoolPtr> ComponentPoolList;
        typedef ComponentPoolList::value_type                         ComponentPoolPair;

    public:
        ComponentSystem();
        ~ComponentSystem();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the component system.
        bool Initialize(Context& context);

        // Creates a component.
        template<typename Type>
        Type* Create(EntityHandle handle);

        // Lookups a component.
        template<typename Type>
        Type* Lookup(EntityHandle handle);

        // Removes a component.
        template<typename Type>
        bool Remove(EntityHandle handle);

        // Gets the begin iterator.
        template<typename Type>
        typename ComponentPool<Type>::ComponentIterator Begin();

        // Gets the end iterator.
        template<typename Type>
        typename ComponentPool<Type>::ComponentIterator End();

        // Gets a component pool.
        template<typename Type>
        ComponentPool<Type>* GetPool();

    private:
        // Creates a component type.
        template<typename Type>
        ComponentPool<Type>* CreatePool();

    private:
        // Called when an entity needs to be finalized.
        bool OnEntityFinalize(EntityHandle handle);

        // Called when an entity gets destroyed.
        void OnEntityDestroyed(EntityHandle handle);

    private:
        // Component pools.
        ComponentPoolList m_pools;

        // Event receivers.
        Receiver<bool(EntityHandle)> m_entityFinalize;
        Receiver<void(EntityHandle)> m_entityDestroyed;

        // Context reference.
        Context* m_context;

        // Initialization state.
        bool m_initialized;
    };

    // Template definitions.
    template<typename Type>
    Type* ComponentSystem::Create(EntityHandle handle)
    {
        if(!m_initialized)
            return nullptr;

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();
        assert(pool != nullptr);

        // Create and return the component.
        return pool->Create(handle);
    }

    template<typename Type>
    Type* ComponentSystem::Lookup(EntityHandle handle)
    {
        if(!m_initialized)
            return nullptr;

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();
        assert(pool != nullptr);

        // Lookup and return the component.
        return pool->Lookup(handle);
    }

    template<typename Type>
    bool ComponentSystem::Remove(EntityHandle handle)
    {
        if(!m_initialized)
            return false;

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();
        assert(pool != nullptr);

        // Remove a component.
        return pool->Remove(handle);
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentSystem::Begin()
    {
        if(!m_initialized)
            return ComponentPool<Type>::ComponentIterator();

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();
        assert(pool != nullptr);

        // Return the iterator.
        return pool->Begin();
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentSystem::End()
    {
        if(!m_initialized)
            return ComponentPool<Type>::ComponentIterator();

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();
        assert(pool != nullptr);

        // Return the iterator.
        return pool->End();
    }

    template<typename Type>
    ComponentPool<Type>* ComponentSystem::CreatePool()
    {
        assert(m_initialized);

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Create and add pool to the collection.
        auto pool = std::make_unique<ComponentPool<Type>>();
        auto pair = ComponentPoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.insert(std::move(pair));

        assert(result.second == true);

        // Return created pool.
        return reinterpret_cast<ComponentPool<Type>*>(result.first->second.get());
    }

    template<typename Type>
    ComponentPool<Type>* ComponentSystem::GetPool()
    {
        if(!m_initialized)
            return nullptr;

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Find pool by component type.
        auto it = m_pools.find(typeid(Type));

        if(it == m_pools.end())
        {
            return this->CreatePool<Type>();
        }

        // Cast and return the pointer that we already know is a component pool.
        return reinterpret_cast<ComponentPool<Type>*>(it->second.get());
    }
}
