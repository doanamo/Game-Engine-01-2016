#pragma once

#include "Precompiled.hpp"
#include "Resource.hpp"

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
    // Resource pool interface.
    class ResourcePoolInterface
    {
    protected:
        ResourcePoolInterface()
        {
        }

    public:
        virtual ~ResourcePoolInterface()
        {
        }

        virtual void ReleaseUnused() = 0;
    };

    // Resource pool class.
    template<typename Type>
    class ResourcePool : public ResourcePoolInterface
    {
    public:
        // Validate resource type.
        static_assert(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Type declarations.
        typedef std::shared_ptr<Type>                        ResourcePtr;
        typedef std::unordered_map<std::string, ResourcePtr> ResourceList;
        typedef typename ResourceList::value_type            ResourceListPair;
        
    public:
        ResourcePool(ResourceManager& resourceManager);
        ~ResourcePool();

        // Sets the default resource.
        void SetDefault(std::shared_ptr<const Type> resource);

        // Gets the default resource.
        std::shared_ptr<const Type> GetDefault() const;

        // Loads a resource.
        std::shared_ptr<const Type> Load(std::string filename);

        // Releases unused resources.
        void ReleaseUnused();

        // Releases all resources.
        void ReleaseAll();

    private:
        // Resource manager reference.
        ResourceManager& m_resourceManager;

        // List of resources.
        ResourceList m_resources;

        // Default resource.
        std::shared_ptr<const Type> m_default;
    };

    template<typename Type>
    ResourcePool<Type>::ResourcePool(ResourceManager& resourceManager) :
        m_resourceManager(resourceManager),
        m_default(std::make_shared<Type>(&m_resourceManager))
    {
    }

    template<typename Type>
    ResourcePool<Type>::~ResourcePool()
    {
        // Release all resources.
        this->ReleaseAll();
    }

    template<typename Type>
    void ResourcePool<Type>::SetDefault(std::shared_ptr<const Type> resource)
    {
        m_default = resource;
    }

    template<typename Type>
    std::shared_ptr<const Type>  ResourcePool<Type>::GetDefault() const
    {
        return m_default;
    }

    template<typename Type>
    std::shared_ptr<const Type> ResourcePool<Type>::Load(std::string filename)
    {
        // Find the resource.
        auto it = m_resources.find(filename);

        if(it != m_resources.end())
            return it->second;

        // Create and load the new resource instance.
        std::shared_ptr<Type> resource = std::make_shared<Type>(&m_resourceManager);

        if(!resource->Load(filename))
            return m_default;

        // Add resource to the list.
        auto result = m_resources.emplace(filename, std::move(resource));

        assert(result.second == true);

        // Return resource pointer.
        return result.first->second;
    }

    template<typename Type>
    void ResourcePool<Type>::ReleaseUnused()
    {
        // Release unused resources.
        auto it = m_resources.begin();

        while(it != m_resources.end())
        {
            if(it->second.unique())
            {
                // Take out filename string to print it later.
                std::string filename = std::move(it->first);

                // Release the resource.
                it = m_resources.erase(it);

                // Print log message.
                Log() << "Released a resource loaded from \"" << filename << "\" file.";
            }
            else
            {
                ++it;
            }
        }
    }

    template<typename Type>
    void ResourcePool<Type>::ReleaseAll()
    {
        // Release all resources.
        auto it = m_resources.begin();

        while(it != m_resources.end())
        {
            // Take out filename string to print it later.
            std::string filename = std::move(it->first);

            // Release the resource.
            it = m_resources.erase(it);

            // Print log message.
            Log() << "Released a resource loaded from \"" << filename << "\" file.";
        }

        assert(m_resources.empty());
    }

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

    private:
        // Creates a resource pool.
        template<typename Type>
        ResourcePool<Type>* CreatePool();

    private:
        // Resource pools.
        ResourcePoolList m_pools;

        // Initialization state.
        bool m_initialized;
    };

    template<typename Type>
    std::shared_ptr<const Type> ResourceManager::Load(std::string filename)
    {
        if(!m_initialized)
            return nullptr;

        // Validate resource type.
        static_assert(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Get the resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();
        assert(pool != nullptr);

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
        assert(pool != nullptr);

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
        assert(pool != nullptr);

        // Return the default resource.
        return pool->GetDefault();
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::CreatePool()
    {
        assert(m_initialized);

        // Validate resource type.
        static_assert(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Create and add a pool to the collection.
        auto pool = std::make_unique<ResourcePool<Type>>(*this);
        auto pair = ResourcePoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.insert(std::move(pair));

        assert(result.second == true);

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
