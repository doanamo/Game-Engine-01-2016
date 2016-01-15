#include "Precompiled.hpp"
#include "Window.hpp"
#include "Config.hpp"
using namespace System;

namespace
{
    // Log message strings.
    #define LogInitializeError() "Failed to initialize a window! "

    // Instance counter for GLFW library.
    bool LibraryInitialized = false;
    int InstanceCount = 0;

    // Window callbacks.
    void ErrorCallback(int error, const char* description)
    {
        Log() << "GLFW Error: " << description;
    }

    void MoveCallback(GLFWwindow* window, int x, int y)
    {
        // Get event dispatchers.
        Window::EventDispatchers* dispatchers = reinterpret_cast<Window::EventDispatchers*>(glfwGetWindowUserPointer(window));
        assert(dispatchers != nullptr);

        // Send an event.
        Window::Events::Move event;
        event.x = x;
        event.y = y;

        dispatchers->move(event);
    }

    void ResizeCallback(GLFWwindow* window, int width, int height)
    {
        // Get event dispatchers.
        Window::EventDispatchers* dispatchers = reinterpret_cast<Window::EventDispatchers*>(glfwGetWindowUserPointer(window));
        assert(dispatchers != nullptr);

        // Send an event.
        Window::Events::Resize event;
        event.width = width;
        event.height = height;

        dispatchers->resize(event);
    }

    void FocusCallback(GLFWwindow* window, int focused)
    {
        // Get event dispatchers.
        Window::EventDispatchers* dispatchers = reinterpret_cast<Window::EventDispatchers*>(glfwGetWindowUserPointer(window));
        assert(dispatchers != nullptr);

        // Send and event.
        Window::Events::Focus event;
        event.focused = focused > 0;
        
        dispatchers->focus(event);
    }

    void CloseCallback(GLFWwindow* window)
    {
        // Get event dispatchers.
        Window::EventDispatchers* dispatchers = reinterpret_cast<Window::EventDispatchers*>(glfwGetWindowUserPointer(window));
        assert(dispatchers != nullptr);

        // Send and event.
        Window::Events::Close event;
        
        dispatchers->close(event);
    }

    void KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        // Get event dispatchers.
        Window::EventDispatchers* dispatchers = reinterpret_cast<Window::EventDispatchers*>(glfwGetWindowUserPointer(window));
        assert(dispatchers != nullptr);

        // Send an event.
        Window::Events::KeyboardKey event;
        event.key = key;
        event.scancode = scancode;
        event.action = action;
        event.mods = mods;

        dispatchers->keyboardKey(event);
    }

    void TextInputCallback(GLFWwindow* window, unsigned int character)
    {
        // Get event dispatchers.
        Window::EventDispatchers* dispatchers = reinterpret_cast<Window::EventDispatchers*>(glfwGetWindowUserPointer(window));
        assert(dispatchers != nullptr);

        // Send an event.
        Window::Events::TextInput event;
        event.character = character;

        dispatchers->textInput(event);
    }

    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        // Get event dispatchers.
        Window::EventDispatchers* dispatchers = reinterpret_cast<Window::EventDispatchers*>(glfwGetWindowUserPointer(window));
        assert(dispatchers != nullptr);

        // Send an event.
        Window::Events::MouseButton event;
        event.button = button;
        event.action = action;
        event.mods = mods;

        dispatchers->mouseButton(event);
    }

    void MouseScrollCallback(GLFWwindow* window, double offsetx, double offsety)
    {
        // Get event dispatchers.
        Window::EventDispatchers* dispatchers = reinterpret_cast<Window::EventDispatchers*>(glfwGetWindowUserPointer(window));
        assert(dispatchers != nullptr);

        // Send an event.
        Window::Events::MouseScroll event;
        event.offset = offsety;

        dispatchers->mouseScroll(event);
    }

    void CursorPositionCallback(GLFWwindow* window, double x, double y)
    {
        // Get event dispatchers.
        Window::EventDispatchers* dispatchers = reinterpret_cast<Window::EventDispatchers*>(glfwGetWindowUserPointer(window));
        assert(dispatchers != nullptr);

        // Send an event.
        Window::Events::CursorPosition event;
        event.x = x;
        event.y = y;

        dispatchers->cursorPosition(event);
    }

    void CursorEnterCallback(GLFWwindow* window, int entered)
    {
        // Get event dispatchers.
        Window::EventDispatchers* dispatchers = reinterpret_cast<Window::EventDispatchers*>(glfwGetWindowUserPointer(window));
        assert(dispatchers != nullptr);

        // Send an event.
        Window::Events::CursorEnter event;
        event.entered = entered != 0;

        dispatchers->cursorEnter(event);
    }
}

Window::Window() :
    events(m_dispatchers),
    m_window(nullptr),
    m_initialized(false)
{
    // Increase instance count.
    ++InstanceCount;
}

Window::Events::Events(Window::EventDispatchers& dispatchers) :
    move(dispatchers.move), 
    resize(dispatchers.resize),
    focus(dispatchers.focus),
    close(dispatchers.close),
    keyboardKey(dispatchers.keyboardKey),
    textInput(dispatchers.textInput),
    mouseButton(dispatchers.mouseButton),
    mouseScroll(dispatchers.mouseScroll),
    cursorPosition(dispatchers.cursorPosition),
    cursorEnter(dispatchers.cursorEnter)
{
}

Window::~Window()
{
    // Cleanup instance.
    if(m_initialized)
        this->Cleanup();

    // Decrease instance count.
    --InstanceCount;

    // Shutdown GLFW library.
    if(InstanceCount == 0)
    {
        if(LibraryInitialized)
        {
            glfwTerminate();

            LibraryInitialized = false;
        }
    }
}

void Window::Cleanup()
{
    // Destroy the window.
    if(m_window != nullptr)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    // Cleanup event dispatchers.
    m_dispatchers.move.Cleanup();
    m_dispatchers.resize.Cleanup();
    m_dispatchers.focus.Cleanup();
    m_dispatchers.close.Cleanup();
    m_dispatchers.keyboardKey.Cleanup();
    m_dispatchers.textInput.Cleanup();
    m_dispatchers.mouseButton.Cleanup();
    m_dispatchers.mouseScroll.Cleanup();
    m_dispatchers.cursorPosition.Cleanup();
    m_dispatchers.cursorEnter.Cleanup();

    // Reset initialization state.
    m_initialized = false;
}

bool Window::Initialize(Context& context)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Add instance to the context.
    if(context.Has<System::Window>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    context.Set(this);

    // Get required instances.
    Config* config = context.Get<System::Config>();

    if(config == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing Config instance.";
        return false;
    }

    // Initialize GLFW library.
    if(!LibraryInitialized)
    {
        glfwSetErrorCallback(ErrorCallback);

        if(!glfwInit())
        {
            Log() << LogInitializeError() << "Couldn't initialize GLFW library!";
            return false;
        }

        LibraryInitialized = true;
    }

    // Read config variables.
    std::string name = config->Get<std::string>("Window.Name", "Game");
    int width = config->Get<int>("Graphics.Width", 1024);
    int height = config->Get<int>("Graphics.Height", 576);
    bool vsync = config->Get<bool>("Graphics.VSync", true);

    // Create the window.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    if(m_window == nullptr)
    {
        Log() << LogInitializeError() << "Couldn't create the window!";
        return false;
    }

    // Set window user data.
    glfwSetWindowUserPointer(m_window, &m_dispatchers);

    // Add event callbacks.
    glfwSetWindowPosCallback(m_window, MoveCallback);
    glfwSetFramebufferSizeCallback(m_window, ResizeCallback);
    glfwSetWindowFocusCallback(m_window, FocusCallback);
    glfwSetWindowCloseCallback(m_window, CloseCallback);
    glfwSetKeyCallback(m_window, KeyboardKeyCallback);
    glfwSetCharCallback(m_window, TextInputCallback);
    glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
    glfwSetScrollCallback(m_window, MouseScrollCallback);
    glfwSetCursorPosCallback(m_window, CursorPositionCallback);
    glfwSetCursorEnterCallback(m_window, CursorEnterCallback);

    // Make window context current.
    glfwMakeContextCurrent(m_window);

    // Set the swap interval.
    glfwSwapInterval((int)vsync);

    // Initialize GLEW library.
    glewExperimental = GL_TRUE;
    GLenum error = glewInit();

    if(error != GLEW_OK)
    {
        Log() << "GLEW Error: " << glewGetErrorString(error);
        Log() << LogInitializeError() << "Couldn't initialize GLEW library!";
        return false;
    }

    // Check created OpenGL context.
    int glMajor = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MINOR);

    Log() << "Created OpenGL " << glMajor << "." << glMinor << " context.";

    // Success!
    return m_initialized = true;
}

void Window::MakeContextCurrent()
{
    if(!m_initialized)
        return;

    glfwMakeContextCurrent(m_window);
}

void Window::ProcessEvents()
{
    if(!m_initialized)
        return;

    glfwPollEvents();
}

void Window::Present()
{
    if(!m_initialized)
        return;

    glfwSwapBuffers(m_window);
}

void Window::Close()
{
    if(!m_initialized)
        return;

    glfwSetWindowShouldClose(m_window, GL_TRUE);
}

bool Window::IsOpen() const
{
    if(!m_initialized)
        return false;

    return glfwWindowShouldClose(m_window) == 0;
}

bool Window::IsFocused() const
{
    if(!m_initialized)
        return false;

    return glfwGetWindowAttrib(m_window, GLFW_FOCUSED) > 0;
}

int Window::GetWidth() const
{
    if(!m_initialized)
        return 0;

    // Get frame buffer width.
    int width = 0;
    glfwGetFramebufferSize(m_window, &width, nullptr);
    return width;
}

int Window::GetHeight() const
{
    if(!m_initialized)
        return 0;

    // Get frame buffer height.
    int height = 0;
    glfwGetFramebufferSize(m_window, nullptr, &height);
    return height;
}

GLFWwindow* Window::GetPrivate()
{
    return m_window;
}
