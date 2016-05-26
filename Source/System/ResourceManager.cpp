#include "Precompiled.hpp"
#include "ResourceManager.hpp"
#include "Context.hpp"
using namespace System;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the resource manager! "
}

ResourceManager::ResourceManager() :
    m_context(nullptr),
    m_initialized(false)
{
}

ResourceManager::~ResourceManager()
{
    this->Cleanup();
}

void ResourceManager::Cleanup()
{
    if(!m_initialized)
        return;

    // Remove all resource pools.
    Utility::ClearContainer(m_pools);

    // Reset context reference.
    m_context = nullptr;

    // Reset initialization state.
    m_initialized = false;
}

bool ResourceManager::Initialize(Context& context)
{
    Assert(context.resourceManager == nullptr);

    // Cleanup this instance.
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

    // Set context instance.
    context.resourceManager = this;

    // Save context reference.
    m_context = &context;

    // Success!
    return m_initialized = true;
}

void ResourceManager::ReleaseUnused()
{
    if(!m_initialized)
        return;

    // Release all unused resources.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;
        pool->ReleaseUnused();
    }
}

Context* ResourceManager::GetContext()
{
    return m_context;
}
