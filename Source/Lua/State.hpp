#pragma once

#include "Precompiled.hpp"

//
// Lua State
//

namespace Lua
{
    // Forward declarations.
    class State;

    template<size_t, typename... Types>
    struct StackPopper;

    // State class.
    class State : private NonCopyable, public std::enable_shared_from_this<State>
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

        // Parses a script string.
        bool Parse(std::string text);

        // Calls a function from a table.
        template<typename... Types, typename... Arguments>
        typename StackPopper<sizeof...(Types), Types...>::ReturnType Call(std::string function, const Arguments&... arguments);

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
    template<typename... Types, typename... Arguments>
    inline typename StackPopper<sizeof...(Types), Types...>::ReturnType State::Call(std::string function, const Arguments&... arguments)
    {
        // Create a scope guard.
        StackGuard guard(this);

        // Caulcate absolute stack indices for StackValue objects.
        StackValue::AbsoluteIndex(*this, arguments...);

        // Check if we got a table.
        if(!lua_istable(m_state, -1))
        {
            Lua::Push<sizeof...(Types)>(*this, nullptr);
            return Lua::Pop<Types...>(*this);
        }

        // Get the function from a table.
        lua_getfield(m_state, -1, function.c_str());

        if(!lua_isfunction(m_state, -1))
        {
            Lua::Push<sizeof...(Types)>(*this, nullptr);
            return Lua::Pop<Types...>(*this);
        }

        // Push function arguments.
        Lua::Push(*this, arguments...);

        // Call the function.
        const int types = (int)(sizeof...(Types));
        const int arguments = (int)(sizeof...(Arguments));

        if(lua_pcall(m_state, arguments, types, 0) != 0)
        {
            this->PrintError();

            for(int i = 1; i <= types; ++i)
            {
                Assert(Lua::Is<std::nullptr_t>(*this, -i));
            }

            return Lua::Pop<Types...>(*this);
        }

        // Return function results.
        return Lua::Pop<Types...>(*this);
    }
}
