#include "Precompiled.hpp"
#include "State.hpp"
using namespace Lua;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize Lua state! "
}

extern "C"
{
    static int LuaLog(lua_State* lua)
    {
        Assert(lua != nullptr);

        if(lua_isstring(lua, -1))
        {
            Log() << lua_tostring(lua, -1);
        }

        return 0;
    }
}

State::State() :
    m_state(nullptr),
    m_initialized(false)
{
}

State::State(State&& other)
{
    m_state = other.m_state;
    other.m_state = nullptr;

    m_initialized = other.m_initialized;
    other.m_initialized = false;
}

State::~State()
{
    this->Cleanup();
}

void State::Cleanup()
{
    if(!m_initialized)
        return;

    // Cleanup Lua state.
    if(m_state != nullptr)
    {
        lua_close(m_state);
        m_state = nullptr;
    }

    // Reset initialization state.
    m_initialized = false;
}

bool State::Initialize()
{
    this->Cleanup();

    // Setup a cleanup guard.
    SCOPE_GUARD
    (
        if(!m_initialized)
        {
            m_initialized = true;
            this->Cleanup();
        }
    );

    // Create Lua state.
    m_state = luaL_newstate();

    if(m_state == nullptr)
    {
        Log() << LogInitializeError() << "Couldn't create a new state.";
        return false;
    }

    // Load the base library.
    lua_pushcfunction(m_state, luaopen_base);
    lua_pushstring(m_state, "");

    if(lua_pcall(m_state, 1, 0, 0) != 0)
    {
        Log() << LogInitializeError() << "Couldn't load the base library.";
        this->PrintError();
        return false;
    }

    // Register the logging function.
    lua_pushcfunction(m_state, LuaLog);
    lua_setglobal(m_state, "Log");

    // Success!
    Assert(lua_gettop(m_state) == 0);

    return m_initialized = true;
}

bool State::Load(std::string filename)
{
    // Initialize if needed.
    if(!m_initialized)
    {
        if(!this->Initialize())
            return false;
    }

    // Parse the file.
    if(luaL_dofile(m_state, (Build::GetWorkingDir() + filename).c_str()) != 0)
    {
        this->PrintError();
        return false;
    }

    return true;
}

bool State::Parse(std::string text)
{
    // Initialize if needed.
    if(!m_initialized)
    {
        if(!this->Initialize())
            return false;
    }

    // Parse the string.
    if(luaL_dostring(m_state, text.c_str()) != 0)
    {
        this->PrintError();
        return false;
    }

    return true;
}

void State::Pop(int count)
{
    if(!m_initialized)
        return;

    lua_pop(m_state, count);
}

void State::PushGlobal()
{
    if(!m_initialized)
        return;

    lua_pushvalue(m_state, LUA_GLOBALSINDEX);
}

void State::PushVariable(std::string name)
{
    if(!m_initialized)
        return;

    // Parse name tokens.
    auto tokens = Utility::SplitString(name, '.');

    if(tokens.empty())
    {
        lua_pop(m_state, -1);
        lua_pushnil(m_state);
        return;
    }

    // Traverse the table chain.
    for(const std::string& token : tokens)
    {
        // Check if we got a table.
        if(!lua_istable(m_state, -1))
        {
            lua_pop(m_state, 1);
            lua_pushnil(m_state);
            return;
        }

        // Push token key.
        lua_pushstring(m_state, token.c_str());

        // Get table element.
        lua_gettable(m_state, -2);

        // Remove the table.
        lua_remove(m_state, -2);
    }
}

void State::CollectGarbage()
{
    if(!m_initialized)
        return;

    // Collect all garbage at once.
    lua_gc(m_state, LUA_GCCOLLECT, 0);
}

void State::CollectGarbage(float maxTime)
{
    if(!m_initialized)
        return;

    if(maxTime <= 0.0f)
        return;

    // Run garbage collector for a specified time.
    double startTime = glfwGetTime();

    do
    {
        if(lua_gc(m_state, LUA_GCSTEP, 0))
            break;
    }
    while((glfwGetTime() - startTime) < maxTime);
}

void State::PrintStack() const
{
    if(m_state == nullptr)
        return;

    Log() << "Lua stack:";

    // Get the index of the top.
    int top = lua_gettop(m_state);

    if(top == 0)
    {
        Log() << "  0: Empty";
    }

    // Print every stack element.
    for(int i = 1; i <= top; ++i)
    {
        int type = lua_type(m_state, i);

        switch(type)
        {
        case LUA_TSTRING:
            Log() << "  " << i << ": \"" << lua_tostring(m_state, i) << "\" (" << lua_typename(m_state, type) << ")";
            break;

        case LUA_TBOOLEAN:
            Log() << "  " << i << ": " << (lua_toboolean(m_state, i) ? "true" : "false") << " (" << lua_typename(m_state, type) << ")";
            break;

        case LUA_TNUMBER:
            Log() << "  " << i << ": " << lua_tonumber(m_state, i) << " (" << lua_typename(m_state, type) << ")";
            break;

        default:
            Log() << "  " << i << ": n/a (" << lua_typename(m_state, type) << ")";
            break;
        }
    }
}

void State::PrintError()
{
    if(m_state == nullptr)
        return;

    Verify(lua_isstring(m_state, -1), "Expected a string.");

    Log() << "Lua Error: " << lua_tostring(m_state, -1);
    lua_pop(m_state, 1);
}

bool State::IsValid() const
{
    return m_state != nullptr;
}

int State::GetStackSize() const
{
    if(!m_initialized)
        return 0;

    return lua_gettop(m_state);
}

lua_State* State::GetPrivate()
{
    return m_state;
}

State::operator lua_State*()
{
    return m_state;
}
