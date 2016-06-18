#pragma once

#include "Precompiled.hpp"
#include "State.hpp"

//
// Lua Functions
//

inline bool luaL_checkboolean(lua_State* state, int index)
{
    if(lua_isboolean(state, index))
    {
        return lua_toboolean(state, index) != 0;
    }
    else
    {
        luaL_error(state, "Not a boolean.");
        return false;
    }
}

inline bool luaL_optboolean(lua_State* state, int index, bool default)
{
    return luaL_opt(state, luaL_checkboolean, index, default);
}

//
// Stack Value
//
//  Objects used to reference a value on the stack by an index.
//

namespace Lua
{
    // Stack value class.
    struct StackValue
    {
    public:
        StackValue(const int index) :
            m_index(index)
        {
        }

        static void AbsoluteIndex(State& state)
        {
        }

        template<typename Type>
        static void AbsoluteIndex(State& state, const Type& value)
        {
        }

        template<>
        static void AbsoluteIndex(State& state, const StackValue& value)
        {
            Assert(state.IsValid());

            if(value.m_index < 0)
            {
                value.m_index = lua_gettop(state) + (value.m_index + 1);
            }
        }

        template<typename Type, typename... Types>
        static void AbsoluteIndex(State& state, const Type& value, const Types&... values)
        {
            Assert(state.IsValid());
            AbsoluteIndex(state, value);
            AbsoluteIndex(state, values...);
        }

        int GetIndex() const
        {
            return m_index;
        }

    private:
        mutable int m_index;
    };
}

//
// Stack Popper
//
//  Pops multiple values from the stacks and returns them as a tuple.
//

namespace Lua
{
    // Stack popper class.
    template<size_t, typename... Types>
    struct StackPopper
    {
        typedef std::tuple<Types...> ReturnType;

        template<typename Type>
        static std::tuple<Type> CreateTuple(State& state, const int index)
        {
            Assert(state.IsValid());
            return std::make_tuple(Read<Type>(state, index));
        }

        template<typename Type1, typename Type2, typename... Rest>
        static std::tuple<Type1, Type2, Rest...> CreateTuple(State& state, const int index)
        {
            Assert(state.IsValid());
            std::tuple<Type1> head = std::make_tuple(Read<Type1>(state, index));
            return std::tuple_cat(head, CreateTuple<Type2, Rest...>(state, index + 1));
        }

        static ReturnType Apply(State& state)
        {
            Assert(state.IsValid());
            auto value = CreateTuple<Types...>(state, -(int)(sizeof...(Types)));
            lua_pop(state, (int)(sizeof...(Types)));
            return value;
        }
    };

    template<typename Type>
    struct StackPopper<1, Type>
    {
        typedef Type ReturnType;

        static ReturnType Apply(State& state)
        {
            Assert(state.IsValid());
            auto value = Read<Type>(state, -1);
            lua_pop(state, 1);
            return value;
        }
    };

    template<typename... Types>
    struct StackPopper<0, Types...>
    {
        typedef void ReturnType;

        static ReturnType Apply(State& state)
        {
        }
    };
}
