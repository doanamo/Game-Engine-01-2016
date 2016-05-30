#pragma once

#include "Precompiled.hpp"

//
// State Interface
//

namespace Lua
{
    // State interface class.
    class StateInterface : public std::enable_shared_from_this<State>
    {
    public:
        // Checks if the instance is valid.
        virtual bool IsValid() const = 0;

        // Gets the private implementation.
        virtual lua_State* GetPrivate() = 0;

        // Conversion operator.
        virtual operator lua_State*() = 0;
    };
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

        static void AbsoluteIndex(StateInterface& state)
        {
        }

        template<typename Type>
        static void AbsoluteIndex(StateInterface& state, const Type& value)
        {
        }

        template<>
        static void AbsoluteIndex(StateInterface& state, const StackValue& value)
        {
            Assert(state.IsValid());

            if(value.m_index < 0)
            {
                value.m_index = lua_gettop(state) + (value.m_index + 1);
            }
        }

        template<typename Type, typename... Types>
        static void AbsoluteIndex(StateInterface& state, const Type& value, const Types&... values)
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
        static std::tuple<Type> CreateTuple(StateInterface& state, const int index)
        {
            Assert(state.IsValid());
            return std::make_tuple(Read<Type>(state, index));
        }

        template<typename Type1, typename Type2, typename... Rest>
        static std::tuple<Type1, Type2, Rest...> CreateTuple(StateInterface& state, const int index)
        {
            Assert(state.IsValid());
            std::tuple<Type1> head = std::make_tuple(Read<Type1>(state, index));
            return std::tuple_cat(head, CreateTuple<Type2, Rest...>(state, index + 1));
        }

        static ReturnType Apply(StateInterface& state)
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

        static ReturnType Apply(StateInterface& state)
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

        static ReturnType Apply(StateInterface& state)
        {
        }
    };
}

//
// Push()
//  
//  Pushes values on the top of the stack.
//

namespace Lua
{
    inline void Push(StateInterface& state)
    {
    }

    template<typename Type>
    void Push(StateInterface& state, const Type& value);

    template<>
    inline void Push(StateInterface& state, const StackValue& value)
    {
        Assert(state.IsValid());
        lua_pushvalue(state, value.GetIndex());
    }

    template<>
    inline void Push(StateInterface& state, const std::nullptr_t&)
    {
        Assert(state.IsValid());
        lua_pushnil(state);
    }

    template<>
    inline void Push(StateInterface& state, const bool& value)
    {
        Assert(state.IsValid());
        lua_pushboolean(state, value);
    }

    template<>
    inline void Push(StateInterface& state, const int& value)
    {
        Assert(state.IsValid());
        lua_pushnumber(state, value);
    }

    template<>
    inline void Push(StateInterface& state, const float& value)
    {
        Assert(state.IsValid());
        lua_pushnumber(state, value);
    }

    template<>
    inline void Push(StateInterface& state, const double& value)
    {
        Assert(state.IsValid());
        lua_pushnumber(state, value);
    }

    template<>
    inline void Push(StateInterface& state, const std::string& value)
    {
        Assert(state.IsValid());
        lua_pushstring(state, value.c_str());
    }

    template<size_t Size>
    inline void Push(StateInterface& state, const std::nullptr_t&)
    {
        Assert(state.IsValid());

        for(int i = 0; i < Size; ++i)
        {
            lua_pushnil(state);
        }
    }

    template<size_t Size>
    inline void Push(StateInterface& state, const char(&value)[Size])
    {
        Assert(state.IsValid());
        lua_pushstring(state, value);
    }

    template<typename Type, typename... Types>
    inline void Push(StateInterface& state, const Type& value, const Types&... values)
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
    inline void Pop(StateInterface& state, const int count = 1)
    {
        Assert(state.IsValid());
        lua_pop(state, count);
    }

    template<typename... Types>
    inline typename StackPopper<sizeof...(Types), Types...>::ReturnType Pop(StateInterface& state)
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
    Type Read(StateInterface& state, const int index = -1);

    template<>
    inline bool Read(StateInterface& state, const int index)
    {
        Assert(state.IsValid());
        return lua_toboolean(state, index) != 0;
    }

    template<>
    inline int Read(StateInterface& state, const int index)
    {
        Assert(state.IsValid());
        return lua_tointeger(state, index);
    }

    template<>
    inline float Read(StateInterface& state, const int index)
    {
        Assert(state.IsValid());
        return (float)lua_tonumber(state, index);
    }

    template<>
    inline double Read(StateInterface& state, const int index)
    {
        Assert(state.IsValid());
        return lua_tonumber(state, index);
    }

    template<>
    inline std::string Read(StateInterface& state, const int index)
    {
        Assert(state.IsValid());

        if(lua_isnil(state, index))
            return "nil";

        return lua_tostring(state, index);
    }

    template<>
    inline Reference Read<Lua::Reference>(StateInterface& state, const int index)
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
    bool Is(StateInterface& state, const int index = -1);

    template<>
    inline bool Is<std::nullptr_t>(StateInterface& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isnil(state, index) != 0;
    }

    template<>
    inline bool Is<bool>(StateInterface& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isboolean(state, index) != 0;
    }

    template<>
    inline bool Is<int>(StateInterface& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isnumber(state, index) != 0;
    }

    template<>
    inline bool Is<float>(StateInterface& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isnumber(state, index) != 0;
    }

    template<>
    inline bool Is<double>(StateInterface& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isnumber(state, index) != 0;
    }

    template<>
    inline bool Is<std::string>(StateInterface& state, const int index)
    {
        Assert(state.IsValid());
        return lua_isstring(state, index) != 0;
    }
}
