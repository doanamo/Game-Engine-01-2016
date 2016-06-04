#pragma once

#include "Precompiled.hpp"
#include "StackGuard.hpp"
#include "Reference.hpp"
#include "Helpers.hpp"
#include "State.hpp"

//
// Push()
//  
//  Pushes values on the top of the stack.
//

namespace Lua
{
    inline void Push(State& state)
    {
    }

    template<typename Type>
    void Push(State& state, const Type& value)
    {
        static_assert(false, "Not implemented for a given type.");
    }

    template<>
    inline void Push(State& state, const StackValue& value)
    {
        Assert(state.IsValid());
        lua_pushvalue(state, value.GetIndex());
    }

    template<>
    inline void Push(State& state, const std::nullptr_t&)
    {
        Assert(state.IsValid());
        lua_pushnil(state);
    }

    template<>
    inline void Push(State& state, const bool& value)
    {
        Assert(state.IsValid());
        lua_pushboolean(state, value);
    }

    template<>
    inline void Push(State& state, const int& value)
    {
        Assert(state.IsValid());
        lua_pushnumber(state, value);
    }

    template<>
    inline void Push(State& state, const float& value)
    {
        Assert(state.IsValid());
        lua_pushnumber(state, value);
    }

    template<>
    inline void Push(State& state, const double& value)
    {
        Assert(state.IsValid());
        lua_pushnumber(state, value);
    }

    template<>
    inline void Push(State& state, const std::string& value)
    {
        Assert(state.IsValid());
        lua_pushstring(state, value.c_str());
    }

    template<>
    inline void Push(State& state, const Reference& reference)
    {
        Assert(state.IsValid());
        Assert(&state == reference.GetState().get());
        reference.PushOntoStack();
    }

    template<>
    inline void Push(State& state, const std::shared_ptr<const Reference>& reference)
    {
        Assert(state.IsValid());
        Assert(reference != nullptr);
        Assert(&state == reference->GetState().get());
        reference->PushOntoStack();
    }

    template<size_t Size>
    void Push(State& state, const std::nullptr_t&)
    {
        Assert(state.IsValid());

        for(int i = 0; i < Size; ++i)
        {
            lua_pushnil(state);
        }
    }

    template<size_t Size>
    void Push(State& state, const char(&value)[Size])
    {
        Assert(state.IsValid());
        lua_pushstring(state, value);
    }

    template<typename Type, typename... Types>
    void Push(State& state, const Type& value, const Types&... values)
    {
        Assert(state.IsValid());
        Lua::Push(state, value);
        Lua::Push(state, values...);
    }
}

//
// Pop()
//
//  Pops values from the top of the stacks.
//

namespace Lua
{
    inline void Pop(State& state, const int count = 1)
    {
        Assert(state.IsValid());
        lua_pop(state, count);
    }

    template<typename... Types>
    typename StackPopper<sizeof...(Types), Types...>::ReturnType Pop(State& state)
    {
        return StackPopper<sizeof...(Types), Types...>::Apply(state);
    }
}

//
// Read()
//
//  Reads a value on the stacks.
//

namespace Lua
{
    template<typename Type>
    Type Read(State& state, const int index = -1)
    {
        static_assert(false, "Not implemented for a given type.");
    }

    template<>
    inline bool Read(State& state, const int index)
    {
        Assert(state.IsValid());
        return lua_toboolean(state, index) != 0;
    }

    template<>
    inline int Read(State& state, const int index)
    {
        Assert(state.IsValid());
        return lua_tointeger(state, index);
    }

    template<>
    inline float Read(State& state, const int index)
    {
        Assert(state.IsValid());
        return (float)lua_tonumber(state, index);
    }

    template<>
    inline double Read(State& state, const int index)
    {
        Assert(state.IsValid());
        return lua_tonumber(state, index);
    }

    template<>
    inline std::string Read(State& state, const int index)
    {
        Assert(state.IsValid());

        if(lua_isnil(state, index))
            return "nil";

        return lua_tostring(state, index);
    }

    template<>
    inline Reference Read<Lua::Reference>(State& state, const int index)
    {
        Assert(state.IsValid());

        // Clone the value we want referenced.
        // Creating a reference of the value will pop it, but we want the stack intact.
        lua_pushvalue(state, index);

        // Create a reference.
        Lua::Reference reference(state.shared_from_this());
        reference.CreateFromStack();
        return reference;
    }
}

//
// Is()
//
//  Checks if a value is of a given type.
//

namespace Lua
{
    template<typename Type>
    bool Is(State& state, const int index = -1)
    {
        static_assert(false, "Not implemented for a given type.");
    }

    template<>
    inline bool Is<std::nullptr_t>(State& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isnil(state, index) != 0;
    }

    template<>
    inline bool Is<bool>(State& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isboolean(state, index) != 0;
    }

    template<>
    inline bool Is<int>(State& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isnumber(state, index) != 0;
    }

    template<>
    inline bool Is<float>(State& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isnumber(state, index) != 0;
    }

    template<>
    inline bool Is<double>(State& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isnumber(state, index) != 0;
    }

    template<>
    inline bool Is<std::string>(State& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isstring(state, index) != 0;
    }
}
