#include "Precompiled.hpp"
#include "Script.hpp"
using namespace Game::Components;

Script::Script()
{
}

Script::~Script()
{
}

bool Script::Finalize(Game::EntityHandle self, const Context& context)
{
    // Make sure all scripts belong to the same state.
    // TODO

    return true;
}

void Script::AddScript(std::shared_ptr<const Lua::Reference> script)
{
    if(script == nullptr || !script->IsValid())
        return;

    // Get the script state.
    Lua::State& state = *script->GetState();

    // Push the script class.
    script->PushOntoStack();

    SCOPE_GUARD(lua_pop(state, -1));

    if(!lua_istable(state, -1))
        return;

    // Push the instantation method.    
    lua_getfield(state, -1, "New");

    if(!lua_isfunction(state, -1))
    {
        lua_pop(state, -1);
        return;
    }

    // Create new script instance.
    if(lua_pcall(state, 0, 1, 0) != 0)
    {
        state.PrintError();
        return;
    }

    // Create a reference to newly created script instance.
    Lua::Reference reference(script->GetState());
    reference.CreateFromStack();

    // Add new script to the list.
    m_scripts.push_back(std::move(reference));
}

void Script::Call(std::string function)
{
    for(auto& script : m_scripts)
    {
        // Get the script state.
        Lua::State& state = *script.GetState();

        // Stack guard.
        int stack = lua_gettop(state);
        SCOPE_GUARD(lua_settop(state, stack));

        // Push a script instance.
        script.PushOntoStack();

        if(!lua_istable(state, -1))
            continue;

        // Get the script method.
        lua_getfield(state, -1, function.c_str());

        if(!lua_isfunction(state, -1))
            continue;

        // Swap the function with the script instance on the stack.
        lua_insert(state, -2);

        // Call the script method.
        if(lua_pcall(state, 1, 0, 0) != 0)
        {
            state.PrintError();
            continue;
        }
    }
}
