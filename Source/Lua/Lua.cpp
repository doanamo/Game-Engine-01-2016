#include "Precompiled.hpp"
#include "Lua.hpp"

void Lua::Push(lua_State* state)
{
    Assert(state != nullptr);
}

void Lua::Push(lua_State* state, const std::nullptr_t&, int count)
{
    Assert(state != nullptr);

    for(int i = 0; i < count; ++i)
    {
        lua_pushnil(state);
    }
}
