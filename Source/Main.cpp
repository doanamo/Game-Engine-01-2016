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

    if(!glfwInit())
    {
        return -1;
    }

    SCOPE_GUARD(glfwTerminate());

    GLFWwindow* window = glfwCreateWindow(1024, 576, "Game", nullptr, nullptr);
    if(!window)
    {
        return -1;
    }

    glfwMakeContextCurrent(window);

    while(!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
