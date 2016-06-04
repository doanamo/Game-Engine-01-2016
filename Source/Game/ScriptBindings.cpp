#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "Lua/StackGuard.hpp"
#include "System/InputState.hpp"
#include "Context.hpp"

//
// Defined Classes 
//

int InputState_IsKeyDown(lua_State* state)
{

    return 0;
}

int InputState_IsKeyUp(lua_State* state)
{

    return 0;
}

//
// Registering Method
//

bool Game::ScriptSystem::RegisterContext(Context& context)
{
    Assert(m_state != nullptr);
    Assert(m_state->IsValid());

    Lua::State& state = *m_state;
    Lua::StackGuard guard(m_state);

    //
    // Input State
    //

    Assert(context.inputState != nullptr);

    // Create an userdata pointer.
    System::InputState* inputState = (System::InputState*)lua_newuserdata(state, sizeof(System::InputState));
    
    // Create and set the metatable.
    lua_newtable(state);

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

    return true;
}
