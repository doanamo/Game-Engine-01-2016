#pragma once

#include "Precompiled.hpp"
#include "StackGuard.hpp"
#include "Reference.hpp"

//
// Lua State
//

namespace Lua
{
    // Forward declarations.
    class State;

    // Stack value class.
    struct StackValue
    {
    public:
        StackValue(const int index) :
            m_index(index)
        {
        }

        static void AbsoluteIndex(Lua::State& state)
        {
        }

        template<typename Type>
        static void AbsoluteIndex(Lua::State& state, const Type& value);

        template<typename Type, typename... Types>
        static void AbsoluteIndex(Lua::State& state, const Type& value, const Types&... values)
        {
            AbsoluteIndex(value);
            AbsoluteIndex(values...);
        }

        int GetIndex() const
        {
            return m_index;
        }

    private:
        mutable int m_index;
    };

    // Stack popper class.
    template<size_t, typename... Types>
    struct StackPopper
    {
        typedef std::tuple<Types...> ReturnType;

        template<typename Type>
        static std::tuple<Type> CreateTuple(Lua::State& state, const int index)
        {
            return std::make_tuple(state.Read<Type>(index));
        }

        template<typename Type1, typename Type2, typename... Rest>
        static std::tuple<Type1, Type2, Rest...> CreateTuple(Lua::State& state, const int index)
        {
            std::tuple<Type1> head = std::make_tuple(state.Read<Type1>(index));
            return std::tuple_cat(head, CreateTuple<Type2, Rest...>(state, index + 1));
        }

        static ReturnType Apply(Lua::State& state)
        {
            auto value = CreateTuple<Types...>(state, -(int)(sizeof...(Types)));
            lua_pop(state, (int)(sizeof...(Types)));
            return value;
        }
    };

    template<typename Type>
    struct StackPopper<1, Type>
    {
        typedef Type ReturnType;

        static ReturnType Apply(Lua::State& state)
        {
            auto value = state.Read<Type>(-1);
            lua_pop(state, 1);
            return value;
        }
    };

    template<typename... Types>
    struct StackPopper<0, Types...>
    {
        typedef void ReturnType;

        static ReturnType Apply(Lua::State& state)
        {
        }
    };

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

        // Pushes a value onto the stack.
        void Push();
        void Push(const std::nullptr_t&, int count);

        template<typename Type>
        void Push(const Type& value);

        template<size_t Size>
        void Push(const char (&value)[Size]);

        template<typename Type, typename... Types>
        void Push(const Type& value, const Types&... values);

        // Checks if value is of a gived type.
        template<typename Type>
        bool Is(const int index = -1);

        // Reads a value from the stack.
        template<typename Type>
        Type Read(const int index = -1);

        // Pops a value from the top of the stack.
        void Pop(const int count = 1);

        template<typename... Types>
        typename StackPopper<sizeof...(Types), Types...>::ReturnType Pop();

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
    template<typename Type>
    inline void StackValue::AbsoluteIndex(Lua::State& state, const Type& value)
    {
    }

    template<>
    inline void StackValue::AbsoluteIndex(Lua::State& state, const StackValue& value)
    {
        if(value.m_index < 0)
        {
            value.m_index = lua_gettop(state) + (value.m_index + 1);
        }
    }

    template<>
    inline void State::Push(const std::nullptr_t&)
    {
        if(!m_initialized)
            return;

        lua_pushnil(m_state);
    }

    template<>
    inline void State::Push(const StackValue& value)
    {
        if(!m_initialized)
            return;

        lua_pushvalue(m_state, value.GetIndex());
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
    inline void State::Push(const Type& value, const Types&... values)
    {
        this->Push(value);
        this->Push(values...);
    }

    template<>
    inline bool State::Is<std::nullptr_t>(const int index)
    {
        if(!m_initialized)
            return false;

        return lua_isnil(m_state, index) != 0;
    }

    template<>
    inline bool State::Is<bool>(const int index)
    {
        if(!m_initialized)
            return false;

        return lua_isboolean(m_state, index) != 0;
    }

    template<>
    inline bool State::Is<int>(const int index)
    {
        if(!m_initialized)
            return false;

        return lua_isnumber(m_state, index) != 0;
    }

    template<>
    inline bool State::Is<float>(const int index)
    {
        if(!m_initialized)
            return false;

        return lua_isnumber(m_state, index) != 0;
    }

    template<>
    inline bool State::Is<double>(const int index)
    {
        if(!m_initialized)
            return false;

        return lua_isnumber(m_state, index) != 0;
    }

    template<>
    inline bool State::Is<std::string>(const int index)
    {
        if(!m_initialized)
            return false;

        return lua_isstring(m_state, index) != 0;
    }

    template<>
    inline bool State::Read(const int index)
    {
        if(!m_initialized)
            return false;

        return lua_toboolean(m_state, index) != 0;
    }

    template<>
    inline int State::Read(const int index)
    {
        if(!m_initialized)
            return 0;

        return lua_tointeger(m_state, index);
    }

    template<>
    inline float State::Read(const int index)
    {
        if(!m_initialized)
            return 0.0f;

        return (float)lua_tonumber(m_state, index);
    }

    template<>
    inline double State::Read(const int index)
    {
        if(!m_initialized)
            return 0.0;

        return lua_tonumber(m_state, index);
    }

    template<>
    inline std::string State::Read(const int index)
    {
        if(!m_initialized)
            return "";

        if(lua_isnil(m_state, index))
            return "nil";

        return lua_tostring(m_state, index);
    }

    template<>
    inline Lua::Reference State::Read(const int index)
    {
        if(!m_initialized)
            return Lua::Reference();

        // Copy the value we want referenced.
        // Creating a reference of the value will pop it, but we want the stack intact.
        lua_pushvalue(m_state, index);

        // Create a reference.
        Lua::Reference reference(this->shared_from_this());
        reference.CreateFromStack();
        return reference;
    }

    template<typename... Types>
    inline typename StackPopper<sizeof...(Types), Types...>::ReturnType State::Pop()
    {
        return StackPopper<sizeof...(Types), Types...>::Apply(*this);
    }

    template<typename... Types, typename... Arguments>
    inline typename StackPopper<sizeof...(Types), Types...>::ReturnType State::Call(std::string function, const Arguments&... arguments)
    {
        StackGuard guard(this);

        // Caulcate absolute stack indices for StackValue objects.
        StackValue::AbsoluteIndex(*this, arguments...);

        // Check if we got a table.
        if(!lua_istable(m_state, -1))
        {
            this->Push(nullptr, (int)(sizeof...(Types)));
            return this->Pop<Types...>();
        }

        // Get the function from a table.
        lua_getfield(m_state, -1, function.c_str());

        if(!lua_isfunction(m_state, -1))
        {
            this->Push(nullptr, (int)(sizeof...(Types)));
            return this->Pop<Types...>();
        }

        // Push function arguments.
        this->Push(arguments...);

        // Call the function.
        const int types = (int)(sizeof...(Types));
        const int arguments = (int)(sizeof...(Arguments));

        if(lua_pcall(m_state, arguments, types, 0) != 0)
        {
            this->PrintError();

            for(int i = 1; i <= types; ++i)
            {
                Assert(this->Is<std::nullptr_t>(-i));
            }

            return this->Pop<Types...>();
        }

        // Return function results.
        return this->Pop<Types...>();
    }
}
