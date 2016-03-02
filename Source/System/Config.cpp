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
    this->Cleanup();
}

void Config::Cleanup()
{
    if(!m_initialized)
        return;

    // Cleanup Lua state.
    m_lua.Cleanup();

    // Reset table name.
    m_table.empty();

    // Reset initialization state.
    m_initialized = false;
}

bool Config::Initialize()
{
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

    // Initialize Lua state.
    if(!m_lua.Initialize())
    {
        Log() << LogInitializeError() << "Couldn't initialize a Lua state.";
        return false;
    }

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

    // Setup a cleanup guard.
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
