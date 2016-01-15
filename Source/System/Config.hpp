#pragma once

#include "Precompiled.hpp"
#include "Lua/State.hpp"

//
// Config
//

namespace System
{
    // Config class.
    class Config
    {
    public:
        Config();
        ~Config();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the class instance.
        bool Initialize();

        // Loads the config from a file.
        bool Load(std::string filename);

        // Sets the table namespace.
        void SetTable(std::string name);

        // Sets a config variable.
        template<typename Type>
        void Set(std::string name, const Type& value);
        
        // Gets a config variable. 
        template<typename Type>
        Type Get(std::string name, const Type& default);

    private:
        // Lua state.
        Lua::State m_lua;

        // Config table name.
        std::string m_table;
        
        // Initialization state.
        bool m_initialized;
    };

    // Template implementation.
    template<typename Type>
    void Config::Set(std::string name, const Type& value)
    {
        if(!m_initialized)
            return;

        assert("Not implemented!");

        /*
        // Push value reference.
        this->PushReference(name);

        // Set new value.
        reference = value;
        */
    }

    template<typename Type>
    Type Config::Get(std::string name, const Type& default)
    {
        if(!m_initialized)
            return default;

        // Push the global table.
        m_lua.PushGlobal();

        // Push the variable value.
        m_lua.PushValue(m_table + "." + name);

        // Cast and return the value.
        return m_lua.CastValue(default);
    }
};
