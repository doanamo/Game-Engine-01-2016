#include "Precompiled.hpp"
#include "Script.hpp"
#include "Game/Script.hpp"
using namespace Game::Components;

Script::Script()
{
}

Script::~Script()
{
    // Release script instances.
    for(auto script : m_scripts)
    {
        luaL_unref(*m_state, LUA_REGISTRYINDEX, script);
    }

    Utility::ClearContainer(m_scripts);

    // Release script state reference.
    m_state = nullptr;
}

void Script::AddScript(std::shared_ptr<const Game::Script> script)
{
    if(script == nullptr)
        return;

    if(!script->IsValid())
        return;

    // Save state reference from the first script we add.
    if(m_state == nullptr)
    {
        m_state = script->GetState();
    }
    else
    {
        if(m_state != script->GetState())
        {
            Log() << "Trying to add a script from different scripting context!";
            return;
        }
    }

    // Push the script class.
    lua_rawgeti(*m_state, LUA_REGISTRYINDEX, script->GetReference());

    SCOPE_GUARD(lua_pop(*m_state, -1));

    if(!lua_istable(*m_state, -1))
        return;

    // Create new script instance.
    lua_getfield(*m_state, -1, "New");

    if(!lua_isfunction(*m_state, -1))
    {
        lua_pop(*m_state, -1);
        return;
    }

    if(lua_pcall(*m_state, 0, 1, 0) != 0)
    {
        m_state->PrintError();
        return;
    }

    SCOPE_GUARD(lua_pop(*m_state, -1));

    // Create a reference to newly created script instance.
    ScriptReference reference = luaL_ref(*m_state, LUA_REGISTRYINDEX);

    if(reference == LUA_REFNIL)
    {
        Log() << "Couldn't create a Lua reference!";
        return;
    }

    // Add script to the list.
    m_scripts.push_back(reference);
}

void Script::Call(std::string function)
{
    assert(m_state != nullptr);

    for(auto script : m_scripts)
    {
        // Stack guard.
        int stack = lua_gettop(*m_state);
        SCOPE_GUARD(lua_settop(*m_state, stack));

        // Push a script instance.
        lua_rawgeti(*m_state, LUA_REGISTRYINDEX, script);

        if(!lua_istable(*m_state, -1))
            continue;

        // Get the script method.
        lua_getfield(*m_state, -1, function.c_str());

        if(!lua_isfunction(*m_state, -1))
            continue;

        // Swap the function with the
        // script instance on the stack.
        lua_insert(*m_state, -2);

        // Call the script method.
        if(lua_pcall(*m_state, 1, 0, 0) != 0)
        {
            m_state->PrintError();
            continue;
        }
    }
}
