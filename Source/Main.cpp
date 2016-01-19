#include "Precompiled.hpp"
#include "System/Config.hpp"
#include "System/Timer.hpp"
#include "System/Window.hpp"
#include "System/InputState.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/BasicRenderer.hpp"
#include "Game/RenderSystem.hpp"

//
// Main
//

int main(int argc, char* argv[])
{
    Debug::Initialize();
    Build::Initialize();
    Logger::Initialize();

    // Create the main context.
    Context context;

    // Initialize the config.
    System::Config config;
    config.Load("Game.cfg");
    config.SetTable("Config");

    context.Set(&config);

    // Initialize the timer.
    System::Timer timer;
    timer.SetMaxDelta(1.0f / 10.0f);

    context.Set(&timer);

    // Initialize the window.
    System::Window window;
    if(!window.Initialize(context))
        return -1;

    // Initialize the input state.
    System::InputState inputState;
    if(!inputState.Initialize(context))
        return -1;

    // Initialize the resource manager.
    System::ResourceManager resourceManager;
    if(!resourceManager.Initialize(context))
        return -1;

    // Initialize the basic renderer.
    Graphics::BasicRenderer basicRenderer;
    if(!basicRenderer.Initialize(context))
        return -1;

    // Initialize the render system.
    Game::RenderSystem renderSystem;
    if(!renderSystem.Initialize(context))
        return -1;

    // Reset the timer.
    timer.Reset();

    // Main loop.
    while(window.IsOpen())
    {
        // Release unused resources.
        resourceManager.ReleaseUnused();

        // Update input state before processing events.
        inputState.Update();

        // Process window events.
        window.ProcessEvents();

        // Draw the scene.
        renderSystem.Draw();

        // Present the backbuffer to the window.
        window.Present();

        // Tick the timer.
        timer.Tick();
    }

    return 0;
}
