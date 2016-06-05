#include "Precompiled.hpp"
#include "InputState.hpp"
#include "Context.hpp"
#include "Window.hpp"
using namespace System;

namespace
{
    // Log message strings.
    #define LogInitializeError() "Failed to initialize an input state! "
}

InputState::InputState() :
    m_initialized(false)
{
    this->Reset();

    // Bind event receivers.
    m_keyboardKey.Bind<InputState, &InputState::OnKeyboardKey>(this);
    m_windowFocus.Bind<InputState, &InputState::OnWindowFocus>(this);
}

InputState::~InputState()
{
    this->Cleanup();
}

void InputState::Cleanup()
{
    if(!m_initialized)
        return;

    // Reset input states.
    this->Reset();

    // Unsubscribe event receivers.
    m_keyboardKey.Unsubscribe();
    m_windowFocus.Unsubscribe();

    // Reset initialization state.
    m_initialized = false;
}

bool InputState::Initialize(Context& context)
{
    Assert(context.window != nullptr);
    Assert(context.inputState == nullptr);

    // Cleanup this instance.
    this->Cleanup();

    // Setup a cleanup guard.
    SCOPE_GUARD
    (
        if(!m_initialized)
        {
            m_initialized = true;
            this->Cleanup();
        }
    );

    // Subscribe event receivers.
    context.window->events.keyboardKey.Subscribe(m_keyboardKey);
    context.window->events.focus.Subscribe(m_windowFocus);

    // Set context instance.
    context.inputState = this;

    // Success!
    return m_initialized = true;
}

void InputState::OnKeyboardKey(const Window::Events::KeyboardKey& event)
{
    Assert(m_initialized);
    Assert(0 <= event.key && event.key < KeyboardKeyCount);

    if(event.action == GLFW_PRESS)
    {
        m_keyboardState[event.key] = KeyboardKeyStates::Pressed;
    }
    else
    if(event.action == GLFW_RELEASE)
    {
        m_keyboardState[event.key] = KeyboardKeyStates::Released;
    }
}

void InputState::OnWindowFocus(const Window::Events::Focus& event)
{
    Assert(m_initialized);

    if(!event.focused)
    {
        this->Reset();
    }
}

void InputState::Update()
{
    if(!m_initialized)
        return;

    for(int i = 0; i < KeyboardKeyCount; ++i)
    {
        auto& state = m_keyboardState[i];

        if(state == KeyboardKeyStates::Pressed)
        {
            state = KeyboardKeyStates::PressedRepeat;
        }
        else
        if(state == KeyboardKeyStates::Released)
        {
            state = KeyboardKeyStates::ReleasedRepeat;
        }
    }
}

void InputState::Reset()
{
    for(int i = 0; i < KeyboardKeyCount; ++i)
    {
        m_keyboardState[i] = KeyboardKeyStates::ReleasedRepeat;
    }
}

bool InputState::IsKeyDown(int key, bool repeat)
{
    if(!m_initialized)
        return false;

    if(key < 0 || key >= KeyboardKeyCount)
        return false;

    if(m_keyboardState[key] == KeyboardKeyStates::Pressed)
        return true;

    if(repeat && m_keyboardState[key] == KeyboardKeyStates::PressedRepeat)
        return true;

    return false;
}

bool InputState::IsKeyUp(int key, bool repeat)
{
    if(!m_initialized)
        return true;

    if(key < 0 || key >= KeyboardKeyCount)
        return false;

    if(m_keyboardState[key] == KeyboardKeyStates::Released)
        return true;

    if(repeat && m_keyboardState[key] == KeyboardKeyStates::ReleasedRepeat)
        return true;

    return false;
}
