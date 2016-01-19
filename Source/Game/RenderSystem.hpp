#pragma once

#include "Precompiled.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/BasicRenderer.hpp"

// Forward declarations.
namespace System
{
    class Window;
}

//
// Render System
//

namespace Game
{
    // Render system class.
    class RenderSystem
    {
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

        // Initialization state.
        bool m_initialized;
    };
}
