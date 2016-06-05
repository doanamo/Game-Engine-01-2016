#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"
#include "Context.hpp"
#include "Lua/Lua.hpp"

//
// Entity Handle
//

namespace Lua
{
    // Entity handle push function.
    template<>
    inline void Push(State& state, const Game::EntityHandle& handle)
    {
        Assert(state.IsValid());

        // Create an userdata for the copy of the object.
        Game::EntityHandle* entityHandle = reinterpret_cast<Game::EntityHandle*>(lua_newuserdata(state, sizeof(Game::EntityHandle)));
        *entityHandle = handle;

        // Set the metatable.
        luaL_getmetatable(state, "EntityHandle");
        lua_setmetatable(state, -2);
    }
}

//
// Register Bindings
//

namespace Game
{
    // Registers script bindings to Lua state instance.
    bool RegisterScriptBindings(Lua::State& state, Context& context);
}
