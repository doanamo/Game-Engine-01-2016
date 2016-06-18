#include "Precompiled.hpp"
#include "Bindings.hpp"
#include "Bindings/Math.hpp"
#include "Bindings/System.hpp"
#include "Bindings/Game.hpp"
using namespace Lua;
using namespace Bindings;

bool Bindings::Register(Lua::State& state, Context& context)
{
    Assert(state.IsValid());

    // Create a stack guard.
    Lua::StackGuard guard(state);

    // Register classes.
    Vec2::Register(state, context);
    InputState::Register(state, context);
    KeyboardKeys::Register(state, context);
    EntityHandle::Register(state, context);
    TransformComponent::Register(state, context);
    ComponentSystem::Register(state, context);

    return true;
}
