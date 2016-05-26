#pragma once

#include "Precompiled.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/BasicRenderer.hpp"

// Forward declarations.
struct Context;

namespace System
{
    class Window;
}

//
// Render System
//
//  Draws the game world on the screen.
//

namespace Game
{
    // Forward declarations.
    class ComponentSystem;

    // Render system class.
    class RenderSystem
    {
    public:
        // Type delcarations.
        typedef std::vector<Graphics::BasicRenderer::Sprite::Info> SpriteInfoList;
        typedef std::vector<Graphics::BasicRenderer::Sprite::Data> SpriteDataList;
        typedef std::vector<std::size_t> SpriteSortList;

    public:
        RenderSystem();
        ~RenderSystem();

        // Restores instance to it's previous state.
        void Cleanup();

        // Initialize the render system.
        bool Initialize(Context& context);

        // Draws the scene.
        void Draw();

    private:
        // Context references.
        System::Window*          m_window;
        Graphics::BasicRenderer* m_basicRenderer;
        ComponentSystem*         m_componentSystem;

        // Screen space transform.
        Graphics::ScreenSpace m_screenSpace;

        // Sprite drawing lists.
        SpriteInfoList m_spriteInfo;
        SpriteDataList m_spriteData;
        SpriteSortList m_spriteSort;

        // Initialization state.
        bool m_initialized;
    };
}
