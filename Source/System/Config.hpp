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

        // Sets a config variable.
        template<typename Type>
        void Set(std::string name, const Type& value);
        
        // Gets a config variable. 
        template<typename Type>
        Type Get(std::string name, const Type& default);

    private:
        // Pushes a value on the stack.
        void PushValue(std::string name);

        // Casts a value and consumes it in the process.
        template<typename Type>
        Type CastValue(const Type& default);

    private:
        // Lua state.
        Lua::State m_lua;
        
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

        // Push value by name.
        this->PushValue(name);

        // Cast and return the value.
        return this->CastValue(default);
    }

    template<>
    inline bool Config::CastValue<bool>(const bool& default)
    {
        bool value = default;

        // Cast the value.
        if(lua_isboolean(m_lua, -1))
        {
            value = lua_toboolean(m_lua, -1) != 0;
        }

        // Remove from the stack.
        lua_pop(m_lua, 1);

        // Return the value.
        return value;
    }

    template<>
    inline int Config::CastValue<int>(const int& default)
    {
        int value = default;

        // Cast the value.
        if(lua_isnumber(m_lua, -1))
        {
            value = (int)std::round(lua_tonumber(m_lua, -1));
        }

        // Remove from the stack.
        lua_pop(m_lua, 1);

        // Return the value.
        return value;
    }
};
