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
        template<typename Type>
        void Push(const Type& value);

        template<size_t Size>
        void Push(const char (&value)[Size]);

        template<typename Type, typename... Types>
        void Push(const Type& value, const Types& ...values);

        // Checks if value is of a gived type.
        template<typename Type>
        bool Is(int index = -1);

        // Reads a value from the stack.
        template<typename Type>
        Type Read(int index = -1);

        // Pops a value from the top of the stack.
        void Pop(int count = 1);

        // Pushes global table on the stack.
        void PushGlobal();
       
        // Pushes variable on the stack.
        // Consumes a table from which value will be pushed.
        void PushVariable(std::string name);

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
    template<>
    inline void State::Push(const std::nullptr_t&)
    {
        if(!m_initialized)
            return;

        lua_pushnil(m_state);
    }

    template<>
    inline void State::Push(const bool& value)
    {
        if(!m_initialized)
            return;

        lua_pushboolean(m_state, value);
    }

    template<>
    inline void State::Push(const int& value)
    {
        if(!m_initialized)
            return;

        lua_pushnumber(m_state, value);
    }

    template<>
    inline void State::Push(const float& value)
    {
        if(!m_initialized)
            return;

        lua_pushnumber(m_state, value);
    }

    template<>
    inline void State::Push(const double& value)
    {
        if(!m_initialized)
            return;

        lua_pushnumber(m_state, value);
    }

    template<>
    inline void State::Push(const std::string& value)
    {
        if(!m_initialized)
            return;

        lua_pushstring(m_state, value.c_str());
    }

    template<size_t Size>
    inline void State::Push(const char(&value)[Size])
    {
        if(!m_initialized)
            return;

        lua_pushstring(m_state, value);
    }

    template<typename Type, typename... Types>
    inline void Push(const Type& value, const Types& ...values)
    {
        this->Push(value);
        this->Push(values...);
    }

    template<>
    inline bool State::Read(int index)
    {
        if(!m_initialized)
            return false;

        return lua_toboolean(m_state, index) != 0;
    }

    template<>
    inline int State::Read(int index)
    {
        if(!m_initialized)
            return 0;

        return lua_tointeger(m_state, index);
    }

    template<>
    inline float State::Read(int index)
    {
        if(!m_initialized)
            return 0.0f;

        return (float)lua_tonumber(m_state, index);
    }

    template<>
    inline double State::Read(int index)
    {
        if(!m_initialized)
            return 0.0;

        return lua_tonumber(m_state, index);
    }

    template<>
    inline std::string State::Read(int index)
    {
        if(!m_initialized)
            return "";

        return lua_tostring(m_state, index);
    }

    template<>
    inline bool State::Is<std::nullptr_t>(int index)
    {
        if(!m_initialized)
            return false;

        return lua_isnil(m_state, index) != 0;
    }

    template<>
    inline bool State::Is<bool>(int index)
    {
        if(!m_initialized)
            return false;

        return lua_isboolean(m_state, index) != 0;
    }

    template<>
    inline bool State::Is<int>(int index)
    {
        if(!m_initialized)
            return false;

        return lua_isnumber(m_state, index) != 0;
    }

    template<>
    inline bool State::Is<float>(int index)
    {
        if(!m_initialized)
            return false;

        return lua_isnumber(m_state, index) != 0;
    }

    template<>
    inline bool State::Is<double>(int index)
    {
        if(!m_initialized)
            return false;

        return lua_isnumber(m_state, index) != 0;
    }

    template<>
    inline bool State::Is<std::string>(int index)
    {
        if(!m_initialized)
            return false;

        return lua_isstring(m_state, index) != 0;
    }
}
