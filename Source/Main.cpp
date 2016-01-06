#include "Precompiled.hpp"
#include "System/Config.hpp"

//
// Main
//

int main(int argc, char* argv[])
{
    Debug::Initialize();
    Build::Initialize();
    Logger::Initialize();

    // Create main context.
    Context context;

    // Load config file.
    System::Config config;
    config.Load("Game.cfg");

    context.Set(&config);

    // Read config settings.
    int windowWidth = config.Get<int>("Graphics.Width", 1024);
    int windowHeight = config.Get<int>("Graphics.Height", 576);
    bool verticalSync = config.Get<bool>("Graphics.VSync", true);

    //
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

    glewExperimental = GL_TRUE;
    GLenum error = glewInit();

    if(error != GLEW_OK)
    {
        return -1;
    }

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
