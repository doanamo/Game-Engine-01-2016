#pragma once

#include "Precompiled.hpp"
#include "Window.hpp"

//
// Input State
//

namespace System
{
    // Forward declarations.
    class Window;

    // Input state class.
    class InputState
    {
    public:
        // Constant variables.
        static const int KeyboardKeyCount = GLFW_KEY_LAST + 1;

        // Keyboard key state.
        struct KeyboardKeyStates
        {
            enum Type
            {
                Pressed,
                PressedRepeat,
                Released,
                ReleasedRepeat,
            };
        };

    public:
        InputState();
        ~InputState();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the input state instance.
        bool Initialize(Context& context);

        // Updates the input state.
        // Must be called before input events are pulled!
        void Update();
        
        // Resets the input state.
        void Reset();

        // Checks if a key is down.
        bool IsKeyDown(int key, bool repeat = true);

        // Checks if a key is up.
        bool IsKeyUp(int key, bool repeat = true);

    private:
        // Called when a keyboard key is pressed.
        void OnKeyboardKey(const Window::Events::KeyboardKey& event);

        // Called when the window changes focus.
        void OnWindowFocus(const Window::Events::Focus& event);

    private:
        // Table of keyboard key states.
        KeyboardKeyStates::Type m_keyboardState[KeyboardKeyCount];

        // Event receivers.
        Receiver<void(const Window::Events::KeyboardKey&)> m_keyboardKey;
        Receiver<void(const Window::Events::Focus&)>       m_windowFocus;

        // Initialization state.
        bool m_initialized;
    };
}
