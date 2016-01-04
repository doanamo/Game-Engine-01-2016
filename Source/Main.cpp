#include "Precompiled.hpp"

//
// Main
//

int main(int argc, char* argv[])
{
    Debug::Initialize();
    Build::Initialize();

    lua_State* lua = luaL_newstate();
    SCOPE_GUARD(lua_close(lua));

    return 0;
}
