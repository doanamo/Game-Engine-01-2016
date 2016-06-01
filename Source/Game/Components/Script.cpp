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

    // Push the script table on the stack.
    Lua::Push(state, script);

    // Create a new script instance.
    auto instance = state.Call<Lua::Reference>("New");

    // Add new script to the list.
    m_scripts.push_back(std::move(instance));
}
