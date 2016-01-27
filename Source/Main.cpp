#include "Precompiled.hpp"
#include "System/Config.hpp"
#include "System/Timer.hpp"
#include "System/Window.hpp"
#include "System/InputState.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/BasicRenderer.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/IdentitySystem.hpp"
#include "Game/ScriptSystem.hpp"
#include "Game/RenderSystem.hpp"

#include "Game/Script.hpp"
#include "Graphics/SpriteSheet.hpp"
#include "Game/Components/Transform.hpp"
#include "Game/Components/Render.hpp"

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

    // Initialize the entity system.
    Game::EntitySystem entitySystem;
    if(!entitySystem.Initialize(context))
        return -1;

    // Initialize the component system.
    Game::ComponentSystem componentSystem;
    if(!componentSystem.Initialize(context))
        return -1;

    // Initialize the identity system.
    Game::IdentitySystem identitySystem;
    if(!identitySystem.Initialize(context))
        return -1;

    // Initialize the script system.
    Game::ScriptSystem scriptSystem;
    if(!scriptSystem.Initialize(context))
        return -1;

    // Initialize the render system.
    Game::RenderSystem renderSystem;
    if(!renderSystem.Initialize(context))
        return -1;

    // Create entities.
    {
        auto playerScript = resourceManager.Load<Game::Script>("Data/Scripts/Player.lua");

        auto spriteSheet = resourceManager.Load<Graphics::SpriteSheet>("Data/Character.sprites");

        auto entity = entitySystem.CreateEntity();
        identitySystem.SetEntityName(entity, "Player");

        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        transform->SetPosition(glm::vec2(0.0f, 0.0f));

        auto render = componentSystem.Create<Game::Components::Render>(entity);
        render->SetTexture(spriteSheet->GetTexture());
        render->SetRectangle(spriteSheet->GetSprite("standing_down"));
        render->SetOffset(glm::vec2(-8.0f, 0.0f));
    }

    {
        auto spriteSheet = resourceManager.Load<Graphics::SpriteSheet>("Data/Character.sprites");

        auto entity = entitySystem.CreateEntity();

        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        transform->SetPosition(glm::vec2(-2.0f, 2.0f));

        auto render = componentSystem.Create<Game::Components::Render>(entity);
        render->SetTexture(spriteSheet->GetTexture());
        render->SetRectangle(spriteSheet->GetSprite("friendly"));
        render->SetOffset(glm::vec2(-8.0f, 0.0f));
    }

    {
        auto spriteSheet = resourceManager.Load<Graphics::SpriteSheet>("Data/Character.sprites");

        auto entity = entitySystem.CreateEntity();

        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        transform->SetPosition(glm::vec2(2.0f, 2.0f));

        auto render = componentSystem.Create<Game::Components::Render>(entity);
        render->SetTexture(spriteSheet->GetTexture());
        render->SetRectangle(spriteSheet->GetSprite("friendly"));
        render->SetOffset(glm::vec2(-8.0f, 0.0f));
    }

    {
        auto spriteSheet = resourceManager.Load<Graphics::SpriteSheet>("Data/Character.sprites");

        auto entity = entitySystem.CreateEntity();

        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        transform->SetPosition(glm::vec2(-2.0f, -2.0f));

        auto render = componentSystem.Create<Game::Components::Render>(entity);
        render->SetTexture(spriteSheet->GetTexture());
        render->SetRectangle(spriteSheet->GetSprite("friendly"));
        render->SetOffset(glm::vec2(-8.0f, 0.0f));
    }

    {
        auto spriteSheet = resourceManager.Load<Graphics::SpriteSheet>("Data/Character.sprites");

        auto entity = entitySystem.CreateEntity();

        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        transform->SetPosition(glm::vec2(2.0f, -2.0f));

        auto render = componentSystem.Create<Game::Components::Render>(entity);
        render->SetTexture(spriteSheet->GetTexture());
        render->SetRectangle(spriteSheet->GetSprite("friendly"));
        render->SetOffset(glm::vec2(-8.0f, 0.0f));
    }

    // Reset the timer.
    timer.Reset();

    // Main loop.
    while(window.IsOpen())
    {
        // Get elapsed time since the last frame.
        float timeDelta = timer.GetDelta();

        // Release unused resources.
        resourceManager.ReleaseUnused();

        // Update input state before processing events.
        inputState.Update();

        // Process window events.
        window.ProcessEvents();

        // Process entity commands.
        entitySystem.ProcessCommands();

        // Update script system.
        scriptSystem.Update(timeDelta);

        // Draw the scene.
        renderSystem.Draw();

        // Present the backbuffer to the window.
        window.Present();

        // Tick the timer.
        timer.Tick();
    }

    return 0;
}
