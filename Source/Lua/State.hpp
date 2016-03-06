#pragma once

#include "Precompiled.hpp"
#include "StackGuard.hpp"

//
// Lua State
//

namespace Lua
{
    // State class.
    class State : private NonCopyable
    {
    public:
        State();
        State(State&& other);
        ~State();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the Lua state.
        bool Initialize();

        // Loads a script file.
        bool Load(std::string filename);

        // Pushes a value onto the stack.
        void Push(const std::nullptr_t);
        void Push(const bool value);
        void Push(const int value);
        void Push(const float value);
        void Push(const std::string value);

        // Pushes global table on the stack.
        void PushGlobal();
       
        // Pushes variable on the stack.
        // Consumes a table from which value will be pushed.
        void PushVariable(std::string name);

        // Casts a value and consumes it in the process.
        template<typename Type>
        Type CastValue(const Type& default);

        // Collects all memory garbage.
        void CollectGarbage();

        // Collects memory garbage for a specified time.
        void CollectGarbage(float maxTime);

        // Prints the stack for debugging.
        void PrintStack() const;

        // Prints the error and pops it.
        void PrintError();

        // Checks if the instance is valid.
        bool IsValid() const;

        // Gets the size of the stack.
        int GetStackSize() const;

        // Gets the private implementation.
        lua_State* GetPrivate();

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
        if(!m_initialized)
            return default;

        // Remove from the stack.
        lua_pop(m_state, 1);

        return default;
    }

    template<>
    inline bool State::CastValue<bool>(const bool& default)
    {
        if(!m_initialized)
            return default;

        // Cast the value.
        bool value = default;

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
        if(!m_initialized)
            return default;

        // Cast the value.
        int value = default;

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
        if(!m_initialized)
            return default;

        // Cast the value.
        std::string value = default;

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
