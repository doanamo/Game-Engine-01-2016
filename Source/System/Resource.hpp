#pragma once

#include "Precompiled.hpp"

// Forward declarations.
namespace System
{
    class ResourceManager;
}

//
// Resource
//

namespace System
{
    // Resource base class.
    class Resource : private NonCopyable
    {
    protected:
        Resource(ResourceManager* resourceManager) :
            m_resourceManager(resourceManager)
        {
        }

    public:
        virtual ~Resource()
        {
        }

        // Gets the resource manager.
        // Can return nullptr, which means resource
        // is not bound to any resource manager.
        ResourceManager* GetResourceManager() const
        {
            return m_resourceManager;
        }

    private:
        // Resource manager that owns this resource.
        ResourceManager* m_resourceManager;
    };
}
