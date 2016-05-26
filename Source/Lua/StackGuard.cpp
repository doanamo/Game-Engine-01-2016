#include "Precompiled.hpp"
#include "StackGuard.hpp"
#include "State.hpp"
using namespace Lua;

StackGuard::StackGuard(const std::shared_ptr<Lua::State>& state) :
    StackGuard(state.get())
{
}

StackGuard::StackGuard(Lua::State* state)
{
    if(state != nullptr)
    {
        m_state = *state;
        m_size = lua_gettop(*state);
    }
    else
    {
        m_state = nullptr;
        m_size = 0;
    }
}

StackGuard::StackGuard(StackGuard&& other)
{
    m_state = other.m_state;
    other.m_state = nullptr;

    m_size = other.m_size;
    other.m_size = 0;
}

StackGuard::~StackGuard()
{
    if(m_state != nullptr)
    {
        // Check current stack size.
        Assert(lua_gettop(m_state) >= m_size, "Current stack size smaller than the guard");

        // Restore previous stack size.
        lua_settop(m_state, m_size);
    }
}
