#include "Precompiled.hpp"

//
// Main
//

int main(int argc, char* argv[])
{
    Debug::Initialize();
    Build::Initialize();

    glm::vec2 test(4.0f, 2.0f);

    lua_State* lua = luaL_newstate();
    SCOPE_GUARD(lua_close(lua));

    return 0;
}
