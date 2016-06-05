#pragma once

#include "Precompiled.hpp"

//
// Stack Guard
//
//  Scoped guard object that reverts the stack
//  to it's previous size when destroyed.
//
//  Example:
//      int size = state.GetStackSize();
//      
//      {
//          StackGuard guard(state);
//          lua_pushnil(state);
//          lua_pushnil(state);
//      }
//      
//      Assert(size == state.GetStackSize());
//

namespace Lua
{
    // Forward declarations.
    class State;

    // Stack guard class.
    class StackGuard : private NonCopyable
    {
    public:
        StackGuard(const std::shared_ptr<Lua::State>& state);
        StackGuard(Lua::State& state);
        StackGuard(Lua::State* state);
        ~StackGuard();

    private:
        lua_State* m_state;
        int        m_size;
    };
}
