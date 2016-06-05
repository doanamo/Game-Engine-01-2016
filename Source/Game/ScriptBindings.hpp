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

        // Create an userdata for the object copy.
        void* memory = lua_newuserdata(state, sizeof(Game::EntityHandle));
        auto* object = new (memory) Game::EntityHandle(handle);

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
