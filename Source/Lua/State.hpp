#pragma once

#include "Precompiled.hpp"

//
// Lua State
//

namespace Lua
{
    // State class.
    class State
    {
    public:
        State();
        ~State();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the Lua state.
        bool Initialize();

        // Loads a script file.
        bool Load(std::string filename);

        // Pushes global table on the stack.
        void PushGlobal();
       
        // Pushes value on the stack.
        // Consumes a table from which value will be pushed.
        void PushValue(std::string name);

        // Casts a value and consumes it in the process.
        template<typename Type>
        Type CastValue(const Type& default);

        // Prints the stack for debugging.
        void PrintStack() const;

        // Conversion operator.
        operator lua_State*();

    private:
        // Virtual machine state.
        lua_State* m_state;

        // Initialization state.
        bool m_initialized;
    };

    // Template definitions.
    template<typename Type>
    inline Type State::CastValue(const Type& default)
    {
        // Remove from the stack.
        lua_pop(m_state, 1);

        return default;
    }

    template<>
    inline bool State::CastValue<bool>(const bool& default)
    {
        bool value = default;

        // Cast the value.
        if(lua_isboolean(m_state, -1))
        {
            value = lua_toboolean(m_state, -1) != 0;
        }

        // Remove from the stack.
        lua_pop(m_state, 1);

        // Return the value.
        return value;
    }

    template<>
    inline int State::CastValue<int>(const int& default)
    {
        int value = default;

        // Cast the value.
        if(lua_isnumber(m_state, -1))
        {
            value = (int)std::round(lua_tonumber(m_state, -1));
        }

        // Remove from the stack.
        lua_pop(m_state, 1);

        // Return the value.
        return value;
    }

    template<>
    inline std::string State::CastValue<std::string>(const std::string& default)
    {
        std::string value = default;

        // Cast the value.
        if(lua_isstring(m_state, -1))
        {
            value = lua_tostring(m_state, -1);
        }

        // Remove from the stack.
        lua_pop(m_state, 1);

        // Return the value.
        return value;
    }
}
