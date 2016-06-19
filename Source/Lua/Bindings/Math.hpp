#pragma once

#include "Precompiled.hpp"
#include "Lua/State.hpp"
#include "Context.hpp"

//
// Vector 2D
//

namespace Lua
{
    namespace Bindings
    {
        namespace Vec2
        {
            // Helper function.
            glm::vec2* Push(lua_State* state);
            glm::vec2* Check(lua_State* state, int index);

            // Meta methods.
            int New(lua_State* state);
            int Call(lua_State* state);
            int Index(lua_State* state);
            int NewIndex(lua_State* state);
            int Add(lua_State* state);
            int Subtract(lua_State* state);
            int Multiply(lua_State* state);
            int Divide(lua_State* state);
            int Equals(lua_State* state);

            // Class methods.
            int Length(lua_State* state);
            int LengthSqr(lua_State* state);
            int Truncate(lua_State* state);
            int Normalize(lua_State* state);

            // Registers Lua bindings.
            void Register(Lua::State& state, Context& context);
        }
    }
}
