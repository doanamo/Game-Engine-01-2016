#pragma once

#include "Precompiled.hpp"

namespace Lua
{
    // Pushes a value onto the stack.
    void Push(lua_State* state);
    void Push(lua_State* state, const std::nullptr_t&, int count);

    template<typename Type>
    void Push(lua_State* state, const Type& value);

    template<size_t Size>
    void Push(lua_State* state, const char (&value)[Size]);

    template<typename Type, typename... Types>
    void Push(lua_State* state, const Type& value, const Types&... values);

    // Checks if value is of a gived type.
    template<typename Type>
    bool Is(lua_State* state, const int index = -1);
}

// Template definitions.
template<>
inline void Lua::Push(lua_State* state, const std::nullptr_t&)
{
    Assert(state != nullptr);
    lua_pushnil(state);
}

template<>
inline void Lua::Push(lua_State* state, const bool& value)
{
    Assert(state != nullptr);
    lua_pushboolean(state, value);
}

template<>
inline void Lua::Push(lua_State* state, const int& value)
{
    Assert(state != nullptr);
    lua_pushnumber(state, value);
}

template<>
inline void Lua::Push(lua_State* state, const float& value)
{
    Assert(state != nullptr);
    lua_pushnumber(state, value);
}

template<>
inline void Lua::Push(lua_State* state, const double& value)
{
    Assert(state != nullptr);
    lua_pushnumber(state, value);
}

template<>
inline void Lua::Push(lua_State* state, const std::string& value)
{
    Assert(state != nullptr);
    lua_pushstring(state, value.c_str());
}

template<size_t Size>
inline void Lua::Push(lua_State* state, const char(&value)[Size])
{
    Assert(state != nullptr);
    lua_pushstring(state, value);
}

template<typename Type, typename... Types>
inline void Lua::Push(lua_State* state, const Type& value, const Types&... values)
{
    Assert(state != nullptr);
    Lua::Push(state, value);
    Lua::Push(state, values...);
}

template<>
inline bool Lua::Is<std::nullptr_t>(lua_State* state, const int index)
{
    Assert(state != nullptr);
    return lua_isnil(state, index) != 0;
}

template<>
inline bool Lua::Is<bool>(lua_State* state, const int index)
{
    Assert(state != nullptr);
    return lua_isboolean(state, index) != 0;
}

template<>
inline bool Lua::Is<int>(lua_State* state, const int index)
{
    Assert(state != nullptr);
    return lua_isnumber(state, index) != 0;
}

template<>
inline bool Lua::Is<float>(lua_State* state, const int index)
{
    Assert(state != nullptr);
    return lua_isnumber(state, index) != 0;
}

template<>
inline bool Lua::Is<double>(lua_State* state, const int index)
{
    Assert(state != nullptr);
    return lua_isnumber(state, index) != 0;
}

template<>
inline bool Lua::Is<std::string>(lua_State* state, const int index)
{
    Assert(state != nullptr);
    return lua_isstring(state, index) != 0;
}
