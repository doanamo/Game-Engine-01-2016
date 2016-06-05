#include "Precompiled.hpp"
#include "Script.hpp"
#include "Game/ScriptBindings.hpp"
using namespace Game::Components;

Script::Script()
{
}

Script::~Script()
{
}

bool Script::Finalize(Game::EntityHandle self, const Context& context)
{
    for(auto& script : m_scripts)
    {
        // Get the scripting state.
        Lua::State& state = *script.GetState();
        Lua::StackGuard guard(&state);

        // Push a script instance on the stack.
        Lua::Push(state, script);

        // Call the script finalize method.
        if(!state.Call<bool>("Finalize", Lua::StackValue(-1), self))
            return false;
    }

    return true;
}

void Script::AddScript(std::shared_ptr<const Lua::Reference> script)
{
    if(script == nullptr || !script->IsValid())
        return;

    // Get the scripting state.
    Lua::State& state = *script->GetState();
    Lua::StackGuard guard(&state);

    // Push the script table on the stack.
    Lua::Push(state, script);

    // Create a new script instance.
    auto instance = state.Call<Lua::Reference>("New");

    // Add new script to the list.
    m_scripts.push_back(std::move(instance));
}
