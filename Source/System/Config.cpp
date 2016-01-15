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

    // Reset table name.
    m_table.empty();

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

    // Success!
    Log() << "Loaded a config from \"" << filename << "\" file.";

    return success = true;
}

void Config::SetTable(std::string name)
{
    if(!m_initialized)
        return;

    m_table = name;
}
