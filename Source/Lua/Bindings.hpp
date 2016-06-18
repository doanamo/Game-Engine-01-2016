#pragma once

#include "Precompiled.hpp"
#include "Context.hpp"
#include "Lua.hpp"

//
// Lua Bindings
//

namespace Lua
{
    namespace Bindings
    {
        // Registers Lua bindings.
        bool Register(Lua::State& state, Context& context);
    }
}
