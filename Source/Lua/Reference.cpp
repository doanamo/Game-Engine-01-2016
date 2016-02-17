#include "Precompiled.hpp"
#include "Reference.hpp"
#include "System/ResourceManager.hpp"
#include "Game/ScriptSystem.hpp"
using namespace Lua;

namespace
{
    // Log error message.
    #define LogLoadError(filename) "Failed to load a reference from \"" << filename << "\" file! "
}

Reference::Reference(System::ResourceManager* resourceManager) :
    System::Resource(resourceManager),
    m_state(nullptr),
    m_reference(LUA_REFNIL)
{
}

Reference::Reference(const std::shared_ptr<Lua::State>& state) :
    System::Resource(nullptr),
    m_state(state),
    m_reference(LUA_REFNIL)
{
}

Reference::Reference(const Reference& other) :
    System::Resource(nullptr),
    m_state(other.m_state),
    m_reference(LUA_REFNIL)
{
    // Create a new reference.
    other.Push();
    this->Create();
}

Reference::~Reference()
{
    this->Release();
}

void Reference::Create()
{
    if(!this->IsValid())
        return;

    m_reference = luaL_ref(*m_state, LUA_REGISTRYINDEX);
}

void Reference::Push() const
{
    if(!this->IsValid())
        return;

    lua_rawgeti(*m_state, LUA_REGISTRYINDEX, m_reference);
}

void Reference::Release()
{
    // Release registered reference.
    if(m_reference != LUA_REFNIL)
    {
        assert(m_state->IsValid());
        luaL_unref(*m_state, LUA_REGISTRYINDEX, m_reference);
        m_reference = LUA_REFNIL;
    }
}

bool Reference::Load(std::string filename)
{
    this->Release();

    // Acquire state reference if missing.
    if(m_state == nullptr)
    {
        // Get the resource manager.
        System::ResourceManager* resourceManager = this->GetResourceManager();

        if(resourceManager == nullptr)
        {
            Log() << LogLoadError(filename) << "Instance has no resource manager bound.";
            return false;
        }

        // Get the context.
        Context* context = resourceManager->GetContext();

        if(context == nullptr)
        {
            Log() << LogLoadError(filename) << "Resource manager is missing the context.";
            return false;
        }

        // Get the script system.
        Game::ScriptSystem* scriptSystem = context->Get<Game::ScriptSystem>();

        if(scriptSystem == nullptr)
        {
            Log() << LogLoadError(filename) << "Context is missing ScriptSystem instance.";
            return false;
        }

        // Get the scripting state.
        m_state = scriptSystem->GetState();

        if(m_state == nullptr)
        {
            Log() << LogLoadError(filename) << "Lua state reference is invalid.";
            return false;
        }
    }

    // Check if state is valid.
    if(!m_state->IsValid())
    {
        Log() << LogLoadError(filename) << "Lua state is invalid.";
        return false;
    }

    // Load the script file.
    if(luaL_loadfile(*m_state, (Build::GetWorkingDir() + filename).c_str()) != 0)
    {
        Log() << LogLoadError(filename) << "Couldn't load the file.";
        return false;
    }

    // Execute the script.
    if(lua_pcall(*m_state, 0, 1, 0) != 0)
    {
        Log() << LogLoadError(filename) << "Couldn't execute the script.";
        m_state->PrintError();
        return false;
    }

    // Create a reference.
    // No error checks needed.
    m_reference = luaL_ref(*m_state, LUA_REGISTRYINDEX);

    // Success!
    Log() << "Loaded a script from \"" << filename << "\" file.";

    return true;
}

std::shared_ptr<Lua::State> Reference::GetState() const
{
    return m_state;
}

Reference::ReferenceID Reference::GetReference() const
{
    return m_reference;
}

bool Reference::IsValid() const
{
    return m_state != nullptr && m_state->IsValid();
}
