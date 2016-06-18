#pragma once

#include "Precompiled.hpp"
#include "Lua/State.hpp"
#include "Context.hpp"

// Forward declarations.
namespace System
{
    class InputState;
}

//
// Input System
//

namespace Lua
{
    namespace Bindings
    {
        namespace InputState
        {
            // Helper functions.
            System::InputState* Check(lua_State* state, int index);

            // Class methods.
            int IsKeyDown(lua_State* state);
            int IsKeyUp(lua_State* state);

            // Registers Lua bindings.
            void Register(Lua::State& state, Context& context);
        }
    }
}

//
// Keyboard Keys
//

namespace Lua
{
    namespace Bindings
    {
        namespace KeyboardKeys
        {
            // Registers Lua bindings.
            void Register(Lua::State& state, Context& context);
        }
    }
}
