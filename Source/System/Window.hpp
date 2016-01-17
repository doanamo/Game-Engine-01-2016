#pragma once

#include "Precompiled.hpp"

//
// Window
//
//  Creates and manages an application window, including the OpenGL context.
//  Supports multiple windows and contexts.
//
//  Example usage:
//      System::Window window;
//      window.Initialize(/* ... */);
//      
//      while(window.IsOpen())
//      {
//          window.ProcessEvents();
//          
//          /* ... */
//          
//          window.Present();
//      }
//
//  Binding events:
//      Class instance;
//      void Class::OnKeyboardKey(const Window::Events::KeyboardKey& event) { /*...*/ }
//      
//      Receiver<void(const Window::Events::KeyboardKey&)> receiver;
//      receiver.Bind<InputState, &InputState::OnKeyboardKey>(&instance);
//      
//      window.events.keyboardKey.Subscribe(receiver);
//

namespace System
{
    // Window class.
    class Window
    {
    public:
        Window();
        ~Window();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the window instance.
        bool Initialize(Context& context);

        // Makes window's context current.
        void MakeContextCurrent();

        // Processes window events.
        void ProcessEvents();

        // Presents backbuffer content to the window.
        void Present();

        // Closes the window.
        void Close();

        // Checks if window is open.
        bool IsOpen() const;

        // Checks if window is focused.
        bool IsFocused() const;

        // Gets window's width.
        int GetWidth() const;

        // Gets window'd height.
        int GetHeight() const;

        // Gets window's private data.
        GLFWwindow* GetPrivate();

    public:
        // Public event dispatchers.
        struct EventDispatchers;

        struct Events
        {
            // Constructor.
            Events(EventDispatchers& dispatchers);

            // Move signal.
            struct Move
            {
                int x;
                int y;
            };

            DispatcherBase<void(const Move&)>& move;

            // Resize signal.
            struct Resize
            {
                int width;
                int height;
            };

            DispatcherBase<void(const Resize&)>& resize;

            // Focus signal.
            struct Focus
            {
                bool focused;
            };

            DispatcherBase<void(const Focus&)>& focus;

            // Close signal.
            struct Close
            {
            };

            DispatcherBase<void(const Close&)>& close;

            // Keyboard key signal.
            struct KeyboardKey
            {
                int key;
                int scancode;
                int action;
                int mods;
            };

            DispatcherBase<void(const KeyboardKey&)>& keyboardKey;

            // Text input signal.
            struct TextInput
            {
                unsigned int character;
            };

            DispatcherBase<void(const TextInput&)>& textInput;

            // Mouse button signal.
            struct MouseButton
            {
                int button;
                int action;
                int mods;
            };

            DispatcherBase<void(const MouseButton&)>& mouseButton;

            // Mouse scroll signal.
            struct MouseScroll
            {
                double offset;
            };

            DispatcherBase<void(const MouseScroll&)>& mouseScroll;

            // Cursor position signal.
            struct CursorPosition
            {
                double x;
                double y;
            };

            DispatcherBase<void(const CursorPosition&)>& cursorPosition;

            // Cursor enter signal.
            struct CursorEnter
            {
                bool entered;
            };

            DispatcherBase<void(const CursorEnter&)>& cursorEnter;
        } events;

        // Private event dispatchers.
        struct EventDispatchers
        {
            Dispatcher<void(const Events::Move&)>           move;
            Dispatcher<void(const Events::Resize&)>         resize;
            Dispatcher<void(const Events::Focus&)>          focus;
            Dispatcher<void(const Events::Close&)>          close;
            Dispatcher<void(const Events::KeyboardKey&)>    keyboardKey;
            Dispatcher<void(const Events::TextInput&)>      textInput;
            Dispatcher<void(const Events::MouseButton&)>    mouseButton;
            Dispatcher<void(const Events::MouseScroll&)>    mouseScroll;
            Dispatcher<void(const Events::CursorPosition&)> cursorPosition;
            Dispatcher<void(const Events::CursorEnter&)>    cursorEnter;
        };

    private:
        // Window implementation.
        GLFWwindow* m_window;

        // Event dispatchers.
        EventDispatchers m_dispatchers;

        // Initialization state.
        bool m_initialized;
    };
}
