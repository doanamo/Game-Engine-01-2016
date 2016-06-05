#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "Lua/StackGuard.hpp"
#include "System/InputState.hpp"
#include "Context.hpp"

//
// Lua Helpers
//

bool luaL_checkboolean(lua_State* state, int index)
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

bool luaL_optboolean(lua_State* state, int index, bool default)
{
    return luaL_opt(state, luaL_checkboolean, index, default);
}

//
// Input System
//

int InputState_IsKeyDown(lua_State* state)
{
    // Get the userdata pointer.
    System::InputState** inputState = reinterpret_cast<System::InputState**>(luaL_checkudata(state, 1, "InputState"));
    Assert(inputState != nullptr && *inputState != nullptr);

    // Call the method and push the result.
    int key = luaL_checkint(state, 2);
    bool repeat = luaL_optboolean(state, 3, true);
    bool result = (*inputState)->IsKeyDown(key, repeat);
    lua_pushboolean(state, result);

    return 1;
}

int InputState_IsKeyUp(lua_State* state)
{
    // Get the userdata pointer.
    System::InputState** inputState = reinterpret_cast<System::InputState**>(luaL_checkudata(state, 1, "InputState"));
    Assert(inputState != nullptr && *inputState != nullptr);

    // Call the method and push the result.
    int key = luaL_checkint(state, 2);
    bool repeat = luaL_optboolean(state, 3, true);
    bool result = (*inputState)->IsKeyUp(key, repeat);
    lua_pushboolean(state, result);

    return 0;
}

void InputState_Register(Lua::State& state, Context& context)
{
    Assert(state.IsValid());
    Assert(context.inputState != nullptr);

    // Create an userdata pointer.
    System::InputState** inputState = reinterpret_cast<System::InputState**>(lua_newuserdata(state, sizeof(System::InputState)));
    *inputState = context.inputState;
    
    // Create and set the metatable.
    luaL_newmetatable(state, "InputState");

    lua_pushliteral(state, "__index");
    lua_pushvalue(state, -2);
    lua_rawset(state, -3);

    lua_pushcfunction(state, InputState_IsKeyDown);
    lua_setfield(state, -2, "IsKeyDown");

    lua_pushcfunction(state, InputState_IsKeyUp);
    lua_setfield(state, -2, "IsKeyUp");

    lua_setmetatable(state, -2);

    // Register as a global variable.
    lua_setfield(state, LUA_GLOBALSINDEX, "InputState");
}

//
// Registering Method
//

bool Game::ScriptSystem::RegisterContext(Context& context)
{
    Assert(m_state != nullptr);
    Assert(m_state->IsValid());

    // Create a stack guard.
    Lua::StackGuard guard(m_state);

    // Register classes.
    InputState_Register(*m_state, context);

    return true;
}
