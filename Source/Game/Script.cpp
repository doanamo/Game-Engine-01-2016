#include "Precompiled.hpp"
#include "Script.hpp"
#include "ScriptSystem.hpp"
#include "System/ResourceManager.hpp"
using namespace Game;

namespace
{
    // Log error message.
    #define LogLoadError(filename) "Failed to load a script from \"" << filename << "\" file! "
}

Script::Script(System::ResourceManager* resourceManager) :
    System::Resource(resourceManager),
    m_reference(LUA_REFNIL),
    m_initialized(false)
{
}

Script::~Script()
{
    if(m_initialized)
        this->Cleanup();
}

void Script::Cleanup()
{
    // Release script class reference.
    if(m_reference != LUA_REFNIL)
    {
        luaL_unref(*m_state, LUA_REGISTRYINDEX, m_reference);
        m_reference = LUA_REFNIL;
    }

    // Reset script state reference.
    m_state = nullptr;

    // Reset initialization state.
    m_initialized = false;
}

bool Script::Load(std::string filename)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

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
    ScriptSystem* scriptSystem = context->Get<ScriptSystem>();

    if(scriptSystem == nullptr)
    {
        Log() << LogLoadError(filename) << "Context is missing ScriptSystem instance.";
        return false;
    }

    // Get the scripting state.
    m_state = scriptSystem->GetState();

    if(m_state == nullptr)
    {
        Log() << LogLoadError(filename) << "Context is missing ScriptSystem instance.";
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
    m_reference = luaL_ref(*m_state, LUA_REGISTRYINDEX);

    if(m_reference == LUA_REFNIL)
    {
        Log() << LogLoadError(filename) << "Couldn't create a reference.";
        return false;
    }

    // Success!
    Log() << "Loaded a script from \"" << filename << "\" file.";

    return m_initialized = true;
}
