#pragma once

// Forward declarations.
namespace System
{
    class Config;
    class Timer;
    class Window;
    class InputState;
    class ResourceManager;
}

namespace Graphics
{
    class BasicRenderer;
}

namespace Game
{
    class EntitySystem;
    class ComponentSystem;
    class IdentitySystem;
    class ScriptSystem;
    class RenderSystem;
}

//
// Context
//

struct Context
{
    // Constructor.
    Context()
    {
        memset(this, 0, sizeof(Context));
    }

    // Context instances.
    System::Config*          config;
    System::Timer*           timer;
    System::Window*          window;
    System::InputState*      inputState;
    System::ResourceManager* resourceManager;
    Graphics::BasicRenderer* basicRenderer;
    Game::EntitySystem*      entitySystem;
    Game::ComponentSystem*   componentSystem;
    Game::IdentitySystem*    identitySystem;
    Game::ScriptSystem*      scriptSystem;
    Game::RenderSystem*      renderSystem;
};
