#pragma once

#include "Precompiled.hpp"
#include "ResourcePool.hpp"

//
// Resource Manager
//
//  Tracks resource references and releases them when no longer needed.
//
//  Example usage:
//      System::ResourceManager resourceManager;
//      resourceManager.Initialize(/* ... */);
//      
//      auto instanceDefault = std::make_shared<Class>();
//      resourceManager->SetDefault<Class>(instanceDefault);
//      
//      {
//          auto instanceA = resourceManager->Load<Class>("Resources/InstanceA");
//          auto instanceB = resourceManager->Load<Class>("Resources/InstanceB");
//      }
//      
//      resourceManager.ReleaseUnused();
//
//  Declaring a resource type:
//      class Class : public System::Resource
//      {
//      public:
//          bool Load(std::string filename) { /* ... */ }
//      };
//

namespace System
{
    // Resource manager class.
    class ResourceManager
    {
    public:
        // Type declarations.
        typedef std::unique_ptr<ResourcePoolInterface>               ResourcePoolPtr;
        typedef std::unordered_map<std::type_index, ResourcePoolPtr> ResourcePoolList;
        typedef ResourcePoolList::value_type                         ResourcePoolPair;

    public:
        ResourceManager();
        ~ResourceManager();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the resource manager.
        bool Initialize(Context& context);

        // Releases unused resources.
        void ReleaseUnused();

        // Loads a resource.
        template<typename Type>
        std::shared_ptr<const Type> Load(std::string filename);

        // Sets the default resource.
        template<typename Type>
        void SetDefault(std::shared_ptr<const Type> default);

        // Gets the default resource.
        template<typename Type>
        std::shared_ptr<const Type> GetDefault() const;

        // Gets a resource pool.
        template<typename Type>
        ResourcePool<Type>* GetPool();

        // Gets the context.
        Context* GetContext();

    private:
        // Creates a resource pool.
        template<typename Type>
        ResourcePool<Type>* CreatePool();

    private:
        // Resource pools.
        ResourcePoolList m_pools;

        // Context reference.
        Context* m_context;

        // Initialization state.
        bool m_initialized;
    };

    // Template definitions.
    template<typename Type>
    std::shared_ptr<const Type> ResourceManager::Load(std::string filename)
    {
        if(!m_initialized)
            return nullptr;

        // Validate resource type.
        static_assert(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Get the resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();

        Assert(pool != nullptr);

        // Delegate to the resource pool.
        return pool->Load(filename);
    }

    template<typename Type>
    void ResourceManager::SetDefault(std::shared_ptr<const Type> default)
    {
        if(!m_initialized)
            return;

        // Validate resource type.
        static_assert(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Get the resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();

        Assert(pool != nullptr);

        // Set the default resource.
        pool->SetDefault(default);
    }

    template<typename Type>
    std::shared_ptr<const Type> ResourceManager::GetDefault() const
    {
        if(!m_initialized)
            return nullptr;

        // Validate resource type.
        static_assert(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Get the resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        
        Assert(pool != nullptr);

        // Return the default resource.
        return pool->GetDefault();
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::CreatePool()
    {
        Assert(m_initialized);

        // Validate resource type.
        static_assert(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Create and add a pool to the collection.
        auto pool = std::make_unique<ResourcePool<Type>>(*this);
        auto pair = ResourcePoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.insert(std::move(pair));

        Assert(result.second == true);

        // Return created pool.
        return reinterpret_cast<ResourcePool<Type>*>(result.first->second.get());
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::GetPool()
    {
        if(!m_initialized)
            return nullptr;

        // Validate resource type.
        static_assert(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Find pool by resource type.
        auto it = m_pools.find(typeid(Type));

        if(it == m_pools.end())
        {
            // Create a new resource pool.
            return this->CreatePool<Type>();
        }

        // Cast and return the pointer that we already know is a resource pool.
        return reinterpret_cast<ResourcePool<Type>*>(it->second.get());
    }
};
