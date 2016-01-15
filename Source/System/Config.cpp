#include "Precompiled.hpp"
#include "Config.hpp"
using namespace System;

namespace
{
    // Log messages.
    #define LogInitializeError() "Failed to initialize a config instance! "
    #define LogLoadError(filename) "Failed to load a config from \"" << filename << "\" file! "
}

Config::Config() :
    m_initialized(false)
{
}

Config::~Config()
{
    if(m_initialized)
        this->Cleanup();
}

void Config::Cleanup()
{
    // Cleanup Lua state.
    m_lua.Cleanup();

    // Reset initialization state.
    m_initialized = false;
}

bool Config::Initialize()
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD_IF(!m_initialized, 
        this->Cleanup()
    );

    // Initialize Lua state.
    if(!m_lua.Initialize())
        return false;

    // Success!
    return m_initialized = true;
}

bool Config::Load(std::string filename)
{
    // Initialize the class instance.
    if(!this->Initialize())
    {
        Log() << LogLoadError(filename) << "Couldn't initialize the instance.";
        return false;
    }

    // Setup the cleanup scope guard.
    bool success = false;

    SCOPE_GUARD_IF(!success,
        this->Cleanup()
    );

    // Load the config file
    if(!m_lua.Load(filename))
    {
        Log() << LogLoadError(filename) << "Couldn't load the file.";
        return false;
    }

    // Validate state.
    lua_getglobal(m_lua, "Config");

    if(!lua_istable(m_lua, -1))
    {
        Log() << LogLoadError(filename) << "Missing \"Config\" table.";
        return false;
    }

    lua_pop(m_lua, 1);
    
    // Success!
    Log() << "Loaded a config from \"" << filename << "\" file.";

    return success = true;
}
