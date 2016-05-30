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
