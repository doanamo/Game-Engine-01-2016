#pragma once

#include "Precompiled.hpp"

//
// Component Pool
//
//  Manages a single type of component.
//  See ComponentSystem for more context.
//

namespace Game
{
    // Component pool interface class.
    class ComponentPoolInterface
    {
    protected:
        ComponentPoolInterface()
        {
        }

    public:
        virtual ~ComponentPoolInterface()
        {
        }

        virtual bool Finalize(EntityHandle handle, const Context& context) = 0;
        virtual bool Remove(EntityHandle handle) = 0;
    };

    // Component pool class.
    template<typename Type>
    class ComponentPool : public ComponentPoolInterface
    {
    public:
        // Check template type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Type declarations.
        typedef std::unordered_map<EntityHandle, Type> ComponentList;
        typedef typename ComponentList::iterator       ComponentIterator;

    public:
        ComponentPool();
        ~ComponentPool();

        // Restores instance to it's original state.
        void Cleanup();

        // Creates a component.
        Type* Create(EntityHandle handle);

        // Lookups a component.
        Type* Lookup(EntityHandle handle);

        // Finalizes a component.
        bool Finalize(EntityHandle handle, const Context& context) override;

        // Removes a component.
        bool Remove(EntityHandle handle) override;

        // Clears all components.
        void Clear();

        // Gets the begin iterator.
        ComponentIterator Begin();

        // Gets the end iterator.
        ComponentIterator End();

    private:
        // List of components.
        ComponentList m_components;
    };

    // Template definitions.
    template<typename Type>
    ComponentPool<Type>::ComponentPool()
    {
    }

    template<typename Type>
    ComponentPool<Type>::~ComponentPool()
    {
    }

    template<typename Type>
    void ComponentPool<Type>::Cleanup()
    {
        *this = ComponentPool<Type>();
    }

    template<typename Type>
    Type* ComponentPool<Type>::Create(EntityHandle handle)
    {
        // Create a new component for this entity handle.
        auto result = m_components.emplace(std::piecewise_construct, std::forward_as_tuple(handle), std::forward_as_tuple());

        if(result.second == false)
            return nullptr;

        // Return a pointer to a newly created component.
        return &result.first->second;
    }

    template<typename Type>
    Type* ComponentPool<Type>::Lookup(EntityHandle handle)
    {
        // Find a component.
        auto result = m_components.find(handle);

        if(result == m_components.end())
            return nullptr;

        // Make sure handles match.
        assert(result->first == handle);

        // Return a pointer to the component.
        return &result->second;
    }

    template<typename Type>
    bool ComponentPool<Type>::Finalize(EntityHandle handle, const Context& context)
    {
        if(m_components.empty())
            return true;

        // Find the component.
        Component* component = this->Lookup(handle);

        // Call the finalizing function.
        if(component != nullptr)
        {
            return component->Finalize(handle, context);
        }

        return true;
    }

    template<typename Type>
    bool ComponentPool<Type>::Remove(EntityHandle handle)
    {
        return m_components.erase(handle) == 1;
    }

    template<typename Type>
    void ComponentPool<Type>::Clear()
    {
        m_components.clear();
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentPool<Type>::Begin()
    {
        return m_components.begin();
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentPool<Type>::End()
    {
        return m_components.end();
    }
}
