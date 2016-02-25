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
    return true;
}

void Script::AddScript(std::shared_ptr<const Lua::Reference> script)
{
    if(script == nullptr || !script->IsValid())
        return;

    // Get the scripting state.
    Lua::State& state = *script->GetState();
    Lua::StackGuard guard(&state);

    // Push the script class.
    script->PushOntoStack();

    if(!lua_istable(state, -1))
        return;

    // Push the instantation method.    
    lua_getfield(state, -1, "New");

    if(!lua_isfunction(state, -1))
        return;

    // Create a script instance.
    if(lua_pcall(state, 0, 1, 0) != 0)
    {
        state.PrintError();
        return;
    }

    // Create a reference to created script instance.
    Lua::Reference reference(script->GetState());
    reference.CreateFromStack();

    // Add new script to the list.
    m_scripts.push_back(std::move(reference));
}

void Script::Call(std::string method)
{
    for(auto& script : m_scripts)
    {
        // Get the scripting state.
        Lua::State& state = *script.GetState();
        Lua::StackGuard guard(&state);

        // Push a script instance.
        script.PushOntoStack();

        if(!lua_istable(state, -1))
            continue;

        // Push the script method.
        lua_getfield(state, -1, method.c_str());

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
