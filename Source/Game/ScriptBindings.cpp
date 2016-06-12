#include "Precompiled.hpp"
#include "ScriptBindings.hpp"
#include "System/InputState.hpp"
#include "Game/Components/Transform.hpp"
#include "Game/EntityHandle.hpp"
#include "Game/ComponentSystem.hpp"

//
// Lua Helpers
//

bool luaL_checkboolean(lua_State* state, int index)
{
    if(lua_isboolean(state, index))
    {
        return lua_toboolean(state, index) != 0;
    }
    else
    {
        luaL_error(state, "Not a boolean.");
        return false;
    }
}

bool luaL_optboolean(lua_State* state, int index, bool default)
{
    return luaL_opt(state, luaL_checkboolean, index, default);
}

//
// Vector 2D
//

glm::vec2* Vec2_Push(lua_State* state)
{
    Assert(state != nullptr);

    // Create an userdata.
    void* memory = lua_newuserdata(state, sizeof(glm::vec2));
    auto* object = new (memory) glm::vec2();

    Assert(memory != nullptr);
    Assert(object != nullptr);

    // Set the metatable.
    luaL_getmetatable(state, "Vec2");
    lua_setmetatable(state, -2);

    return object;
}

glm::vec2* Vec2_Check(lua_State* state, int index)
{
    Assert(state != nullptr);

    // Get the userdata.
    void* memory = luaL_checkudata(state, index, "Vec2");
    auto* object = reinterpret_cast<glm::vec2*>(memory);
    Assert(memory != nullptr && object != nullptr);

    return object;
}

int Vec2_New(lua_State* state)
{
    Assert(state != nullptr);

    // Push a new instance.
    Vec2_Push(state);

    return 1;
}

int Vec2_Index(lua_State* state)
{
    Assert(state != nullptr);
    
    // Return the property.
    glm::vec2* vector = Vec2_Check(state, 1);
    std::string key = luaL_checkstring(state, 2);
    
    if(key == "x")
    {
        lua_pushnumber(state, vector->x);
        return 1;
    }
    else
    if(key == "y")
    {
        lua_pushnumber(state, vector->y);
        return 1;
    }
    else
    {
        lua_pushnil(state);
        return 1;
    }
}

int Vec2_NewIndex(lua_State* state)
{
    Assert(state != nullptr);
    
    // Set the property.
    glm::vec2* vector = Vec2_Check(state, 1);
    std::string key = luaL_checkstring(state, 2);

    if(key == "x")
    {
        vector->x = (float)luaL_checknumber(state, 3);
        return 0;
    }
    else
    if(key == "y")
    {
        vector->y = (float)luaL_checknumber(state, 3);
        return 0;
    }

    return 0;
}

void Vec2_Register(Lua::State& state, Context& context)
{
    Assert(state.IsValid());

    // Create a class metatable.
    luaL_newmetatable(state, "Vec2");

    lua_pushcfunction(state, Vec2_New);
    lua_setfield(state, -2, "New");

    lua_pushcfunction(state, Vec2_Index);
    lua_setfield(state, -2, "__index");

    lua_pushcfunction(state, Vec2_NewIndex);
    lua_setfield(state, -2, "__newindex");

    // Create a secondary metatable.
    lua_newtable(state);

    lua_pushcfunction(state, Vec2_New);
    lua_setfield(state, -2, "__call");

    lua_setmetatable(state, -2);

    // Register as a global variable.
    lua_setfield(state, LUA_GLOBALSINDEX, "Vec2");
}

//
// Input System
//

System::InputState* InputState_Check(lua_State* state, int index)
{
    Assert(state != nullptr);

    // Get the userdata pointer.
    void* memory = luaL_checkudata(state, index, "InputState");
    auto* object = *reinterpret_cast<System::InputState**>(memory);
    Assert(memory != nullptr && object != nullptr);

    return object;
}

int InputState_IsKeyDown(lua_State* state)
{
    Assert(state != nullptr);

    // Get arguments from the stack.
    auto* inputState = InputState_Check(state, 1);
    int   key        = luaL_checkint(state, 2);
    bool  repeat     = luaL_optboolean(state, 3, true);
    
    // Call the method and push the result.
    bool result = inputState->IsKeyDown(key, repeat);
    lua_pushboolean(state, result);

    return 1;
}

int InputState_IsKeyUp(lua_State* state)
{
    Assert(state != nullptr);

    // Get arguments from the stack.
    auto* inputState = InputState_Check(state, 1);
    int   key        = luaL_checkint(state, 2);
    bool  repeat     = luaL_optboolean(state, 3, true);

    // Call the method and push the result.
    bool result = inputState->IsKeyUp(key, repeat);
    lua_pushboolean(state, result);

    return 1;
}

void InputState_Register(Lua::State& state, Context& context)
{
    Assert(state.IsValid());
    Assert(context.inputState != nullptr);

    // Create an userdata pointer.
    void* memory = lua_newuserdata(state, sizeof(System::InputState*));
    auto** pointer = reinterpret_cast<System::InputState**>(memory);
    *pointer = context.inputState;

    // Create and set the metatable.
    luaL_newmetatable(state, "InputState");

    lua_pushliteral(state, "__index");
    lua_pushvalue(state, -2);
    lua_rawset(state, -3);

    lua_pushcfunction(state, InputState_IsKeyDown);
    lua_setfield(state, -2, "IsKeyDown");

    lua_pushcfunction(state, InputState_IsKeyUp);
    lua_setfield(state, -2, "IsKeyUp");

    lua_setmetatable(state, -2);

    // Register as a global variable.
    lua_setfield(state, LUA_GLOBALSINDEX, "InputState");

    // Register the table of keyboard keys.
    lua_newtable(state);

    lua_pushinteger(state, GLFW_KEY_SPACE);
    lua_setfield(state, -2, "Space");

    lua_pushinteger(state, GLFW_KEY_APOSTROPHE);
    lua_setfield(state, -2, "Apostrophe");

    lua_pushinteger(state, GLFW_KEY_COMMA);
    lua_setfield(state, -2, "Comma");

    lua_pushinteger(state, GLFW_KEY_MINUS);
    lua_setfield(state, -2, "Minus");

    lua_pushinteger(state, GLFW_KEY_PERIOD);
    lua_setfield(state, -2, "Period");

    lua_pushinteger(state, GLFW_KEY_SLASH);
    lua_setfield(state, -2, "Slash");

    lua_pushinteger(state, GLFW_KEY_0);
    lua_setfield(state, -2, "0");

    lua_pushinteger(state, GLFW_KEY_1);
    lua_setfield(state, -2, "1");

    lua_pushinteger(state, GLFW_KEY_2);
    lua_setfield(state, -2, "2");

    lua_pushinteger(state, GLFW_KEY_3);
    lua_setfield(state, -2, "3");

    lua_pushinteger(state, GLFW_KEY_4);
    lua_setfield(state, -2, "4");

    lua_pushinteger(state, GLFW_KEY_5);
    lua_setfield(state, -2, "5");

    lua_pushinteger(state, GLFW_KEY_6);
    lua_setfield(state, -2, "6");

    lua_pushinteger(state, GLFW_KEY_7);
    lua_setfield(state, -2, "7");

    lua_pushinteger(state, GLFW_KEY_8);
    lua_setfield(state, -2, "8");

    lua_pushinteger(state, GLFW_KEY_9);
    lua_setfield(state, -2, "9");

    lua_pushinteger(state, GLFW_KEY_SEMICOLON);
    lua_setfield(state, -2, "Semicolon");

    lua_pushinteger(state, GLFW_KEY_EQUAL);
    lua_setfield(state, -2, "Equal");

    lua_pushinteger(state, GLFW_KEY_A);
    lua_setfield(state, -2, "A");

    lua_pushinteger(state, GLFW_KEY_B);
    lua_setfield(state, -2, "B");

    lua_pushinteger(state, GLFW_KEY_C);
    lua_setfield(state, -2, "C");

    lua_pushinteger(state, GLFW_KEY_D);
    lua_setfield(state, -2, "D");

    lua_pushinteger(state, GLFW_KEY_E);
    lua_setfield(state, -2, "E");

    lua_pushinteger(state, GLFW_KEY_F);
    lua_setfield(state, -2, "F");

    lua_pushinteger(state, GLFW_KEY_G);
    lua_setfield(state, -2, "G");

    lua_pushinteger(state, GLFW_KEY_H);
    lua_setfield(state, -2, "H");

    lua_pushinteger(state, GLFW_KEY_I);
    lua_setfield(state, -2, "I");

    lua_pushinteger(state, GLFW_KEY_J);
    lua_setfield(state, -2, "J");

    lua_pushinteger(state, GLFW_KEY_K);
    lua_setfield(state, -2, "K");

    lua_pushinteger(state, GLFW_KEY_L);
    lua_setfield(state, -2, "L");

    lua_pushinteger(state, GLFW_KEY_M);
    lua_setfield(state, -2, "M");

    lua_pushinteger(state, GLFW_KEY_N);
    lua_setfield(state, -2, "N");

    lua_pushinteger(state, GLFW_KEY_O);
    lua_setfield(state, -2, "O");

    lua_pushinteger(state, GLFW_KEY_P);
    lua_setfield(state, -2, "P");

    lua_pushinteger(state, GLFW_KEY_Q);
    lua_setfield(state, -2, "Q");

    lua_pushinteger(state, GLFW_KEY_R);
    lua_setfield(state, -2, "R");

    lua_pushinteger(state, GLFW_KEY_S);
    lua_setfield(state, -2, "S");

    lua_pushinteger(state, GLFW_KEY_T);
    lua_setfield(state, -2, "T");

    lua_pushinteger(state, GLFW_KEY_U);
    lua_setfield(state, -2, "U");

    lua_pushinteger(state, GLFW_KEY_V);
    lua_setfield(state, -2, "V");

    lua_pushinteger(state, GLFW_KEY_W);
    lua_setfield(state, -2, "W");

    lua_pushinteger(state, GLFW_KEY_X);
    lua_setfield(state, -2, "X");

    lua_pushinteger(state, GLFW_KEY_Y);
    lua_setfield(state, -2, "Y");

    lua_pushinteger(state, GLFW_KEY_Z);
    lua_setfield(state, -2, "Z");

    lua_pushinteger(state, GLFW_KEY_LEFT_BRACKET);
    lua_setfield(state, -2, "LeftBracket");

    lua_pushinteger(state, GLFW_KEY_BACKSLASH);
    lua_setfield(state, -2, "Backslash");

    lua_pushinteger(state, GLFW_KEY_RIGHT_BRACKET);
    lua_setfield(state, -2, "RightBracket");

    lua_pushinteger(state, GLFW_KEY_GRAVE_ACCENT);
    lua_setfield(state, -2, "Grave");

    lua_pushinteger(state, GLFW_KEY_ESCAPE);
    lua_setfield(state, -2, "Escape");

    lua_pushinteger(state, GLFW_KEY_ENTER);
    lua_setfield(state, -2, "Enter");

    lua_pushinteger(state, GLFW_KEY_TAB);
    lua_setfield(state, -2, "Tab");

    lua_pushinteger(state, GLFW_KEY_BACKSPACE);
    lua_setfield(state, -2, "Backspace");

    lua_pushinteger(state, GLFW_KEY_INSERT);
    lua_setfield(state, -2, "Insert");

    lua_pushinteger(state, GLFW_KEY_DELETE);
    lua_setfield(state, -2, "Delete");

    lua_pushinteger(state, GLFW_KEY_RIGHT);
    lua_setfield(state, -2, "Right");

    lua_pushinteger(state, GLFW_KEY_LEFT);
    lua_setfield(state, -2, "Left");

    lua_pushinteger(state, GLFW_KEY_DOWN);
    lua_setfield(state, -2, "Down");

    lua_pushinteger(state, GLFW_KEY_UP);
    lua_setfield(state, -2, "Up");

    lua_pushinteger(state, GLFW_KEY_PAGE_UP);
    lua_setfield(state, -2, "PageUp");

    lua_pushinteger(state, GLFW_KEY_PAGE_DOWN);
    lua_setfield(state, -2, "PageDown");

    lua_pushinteger(state, GLFW_KEY_HOME);
    lua_setfield(state, -2, "Home");

    lua_pushinteger(state, GLFW_KEY_END);
    lua_setfield(state, -2, "End");

    lua_pushinteger(state, GLFW_KEY_CAPS_LOCK);
    lua_setfield(state, -2, "CapsLock");

    lua_pushinteger(state, GLFW_KEY_SCROLL_LOCK);
    lua_setfield(state, -2, "ScrollLock");

    lua_pushinteger(state, GLFW_KEY_NUM_LOCK);
    lua_setfield(state, -2, "NumLock");

    lua_pushinteger(state, GLFW_KEY_PRINT_SCREEN);
    lua_setfield(state, -2, "PrintScreen");

    lua_pushinteger(state, GLFW_KEY_PAUSE);
    lua_setfield(state, -2, "Pause");

    lua_pushinteger(state, GLFW_KEY_F1);
    lua_setfield(state, -2, "F1");

    lua_pushinteger(state, GLFW_KEY_F2);
    lua_setfield(state, -2, "F2");

    lua_pushinteger(state, GLFW_KEY_F3);
    lua_setfield(state, -2, "F3");

    lua_pushinteger(state, GLFW_KEY_F4);
    lua_setfield(state, -2, "F4");

    lua_pushinteger(state, GLFW_KEY_F5);
    lua_setfield(state, -2, "F5");

    lua_pushinteger(state, GLFW_KEY_F6);
    lua_setfield(state, -2, "F6");

    lua_pushinteger(state, GLFW_KEY_F7);
    lua_setfield(state, -2, "F7");

    lua_pushinteger(state, GLFW_KEY_F8);
    lua_setfield(state, -2, "F8");

    lua_pushinteger(state, GLFW_KEY_F9);
    lua_setfield(state, -2, "F9");

    lua_pushinteger(state, GLFW_KEY_F10);
    lua_setfield(state, -2, "F10");

    lua_pushinteger(state, GLFW_KEY_F11);
    lua_setfield(state, -2, "F11");

    lua_pushinteger(state, GLFW_KEY_F12);
    lua_setfield(state, -2, "F12");

    lua_pushinteger(state, GLFW_KEY_KP_0);
    lua_setfield(state, -2, "Num0");

    lua_pushinteger(state, GLFW_KEY_KP_1);
    lua_setfield(state, -2, "Num1");

    lua_pushinteger(state, GLFW_KEY_KP_2);
    lua_setfield(state, -2, "Num2");

    lua_pushinteger(state, GLFW_KEY_KP_3);
    lua_setfield(state, -2, "Num3");

    lua_pushinteger(state, GLFW_KEY_KP_4);
    lua_setfield(state, -2, "Num4");

    lua_pushinteger(state, GLFW_KEY_KP_5);
    lua_setfield(state, -2, "Num5");

    lua_pushinteger(state, GLFW_KEY_KP_6);
    lua_setfield(state, -2, "Num6");

    lua_pushinteger(state, GLFW_KEY_KP_7);
    lua_setfield(state, -2, "Num7");

    lua_pushinteger(state, GLFW_KEY_KP_8);
    lua_setfield(state, -2, "Num8");

    lua_pushinteger(state, GLFW_KEY_KP_9);
    lua_setfield(state, -2, "Num9");

    lua_pushinteger(state, GLFW_KEY_KP_DECIMAL);
    lua_setfield(state, -2, "NumDecimal");

    lua_pushinteger(state, GLFW_KEY_KP_DIVIDE);
    lua_setfield(state, -2, "NumDivide");

    lua_pushinteger(state, GLFW_KEY_KP_MULTIPLY);
    lua_setfield(state, -2, "NumMultiply");

    lua_pushinteger(state, GLFW_KEY_KP_SUBTRACT);
    lua_setfield(state, -2, "NumSubtract");

    lua_pushinteger(state, GLFW_KEY_KP_ADD);
    lua_setfield(state, -2, "NumAdd");

    lua_pushinteger(state, GLFW_KEY_KP_ENTER);
    lua_setfield(state, -2, "NumEnter");

    lua_pushinteger(state, GLFW_KEY_KP_EQUAL);
    lua_setfield(state, -2, "NumEqual");

    lua_pushinteger(state, GLFW_KEY_LEFT_SHIFT);
    lua_setfield(state, -2, "LeftShift");

    lua_pushinteger(state, GLFW_KEY_LEFT_CONTROL);
    lua_setfield(state, -2, "LeftControl");

    lua_pushinteger(state, GLFW_KEY_LEFT_ALT);
    lua_setfield(state, -2, "LeftAlt");

    lua_pushinteger(state, GLFW_KEY_LEFT_SUPER);
    lua_setfield(state, -2, "LeftSuper");

    lua_pushinteger(state, GLFW_KEY_RIGHT_SHIFT);
    lua_setfield(state, -2, "RightShift");

    lua_pushinteger(state, GLFW_KEY_RIGHT_CONTROL);
    lua_setfield(state, -2, "RightControl");

    lua_pushinteger(state, GLFW_KEY_RIGHT_ALT);
    lua_setfield(state, -2, "RightAlt");

    lua_pushinteger(state, GLFW_KEY_RIGHT_SUPER);
    lua_setfield(state, -2, "RightSuper");

    lua_pushinteger(state, GLFW_KEY_MENU);
    lua_setfield(state, -2, "Menu");

    lua_setfield(state, LUA_GLOBALSINDEX, "Keys");
}

//
// Entity Handle
//

Game::EntityHandle* EntityHandle_Push(lua_State* state)
{
    Assert(state != nullptr);

    // Create an userdata.
    void* memory = lua_newuserdata(state, sizeof(Game::EntityHandle));
    auto* object = new (memory) Game::EntityHandle();

    Assert(memory != nullptr);
    Assert(object != nullptr);

    // Set the metatable.
    luaL_getmetatable(state, "EntityHandle");
    lua_setmetatable(state, -2);

    return object;
}

Game::EntityHandle* EntityHandle_Check(lua_State* state, int index)
{
    Assert(state != nullptr);

    // Get the userdata object.
    void* memory = luaL_checkudata(state, index, "EntityHandle");
    auto* object = reinterpret_cast<Game::EntityHandle*>(memory);
    Assert(memory != nullptr && object != nullptr);

    return object;
}

int EntityHandle_New(lua_State* state)
{
    Assert(state != nullptr);

    // Push the instance.
    EntityHandle_Push(state);

    return 1;
}

int EntityHandle_Index(lua_State* state)
{
    Assert(state != nullptr);
    
    // Get arguments from the stack.
    auto* handle = EntityHandle_Check(state, 1);
    std::string key = luaL_checkstring(state, 2);

    // Return the property.
    if(key == "identifier")
    {
        lua_pushinteger(state, handle->identifier);
        return 1;
    }
    else
    if(key == "version")
    {
        lua_pushinteger(state, handle->version);
        return 1;
    }
    else
    {
        lua_pushnil(state);
        return 1;
    }
}

int EntityHandle_NewIndex(lua_State* state)
{
    Assert(state != nullptr);
    
    // Get arguments from the stack.
    auto* handle = EntityHandle_Check(state, 1);
    std::string key = luaL_checkstring(state, 2);

    // Set the property.
    if(key == "identifier")
    {
        handle->identifier = luaL_checkinteger(state, 3);
        return 0;
    }
    else
    if(key == "version")
    {
        handle->version = luaL_checkinteger(state, 3);
        return 0;
    }

    return 0;
}

void EntityHandle_Register(Lua::State& state, Context& context)
{
    Assert(state.IsValid());

    // Create a class metatable.
    luaL_newmetatable(state, "EntityHandle");

    lua_pushcfunction(state, EntityHandle_New);
    lua_setfield(state, -2, "New");

    lua_pushcfunction(state, EntityHandle_Index);
    lua_setfield(state, -2, "__index");

    lua_pushcfunction(state, EntityHandle_NewIndex);
    lua_setfield(state, -2, "__newindex");

    // Create a secondary table.
    lua_newtable(state);

    lua_pushcfunction(state, EntityHandle_New);
    lua_setfield(state, -2, "__call");

    lua_setmetatable(state, -2);

    // Register as a global variable.
    lua_setfield(state, LUA_GLOBALSINDEX, "EntityHandle");
}

//
// Transform Component
//

Game::Components::Transform* TransformComponent_Push(lua_State* state, Game::Components::Transform* transform)
{
    Assert(state != nullptr);

    // Create an userdata pointer.
    void* memory = lua_newuserdata(state, sizeof(Game::Components::Transform*));
    auto** pointer = reinterpret_cast<Game::Components::Transform**>(memory);
    *pointer = transform;

    Assert(memory != nullptr);
    Assert(pointer != nullptr);

    // Set the metatable.
    luaL_getmetatable(state, "TransformComponent");
    lua_setmetatable(state, -2);

    return *pointer;
}

Game::Components::Transform* TransformComponent_Check(lua_State* state, int index)
{
    Assert(state != nullptr);

    // Get the userdata pointer.
    void* memory = luaL_checkudata(state, index, "TransformComponent");
    auto* object = *reinterpret_cast<Game::Components::Transform**>(memory);
    Assert(memory != nullptr && object != nullptr);

    return object;
}

int TransformComponent_SetPosition(lua_State* state)
{
    Assert(state != nullptr);

    // Get arguments from the stack.
    auto* transform = TransformComponent_Check(state, 1);
    glm::vec2* position = Vec2_Check(state, 2);

    // Call the method.
    transform->SetPosition(*position);

    return 0;
}

int TransformComponent_GetPosition(lua_State* state)
{
    Assert(state != nullptr);

    // Get arguments from the stack.
    auto* transform = TransformComponent_Check(state, 1);

    // Call the method.
    glm::vec2 position = transform->GetPosition();

    // Push the result.
    *Vec2_Push(state) = position;

    return 1;
}

void TransformComponent_Register(Lua::State& state, Context& context)
{
    Assert(state.IsValid());

    // Create a class metatable.
    luaL_newmetatable(state, "TransformComponent");

    lua_pushliteral(state, "__index");
    lua_pushvalue(state, -2);
    lua_rawset(state, -3);

    lua_pushcfunction(state, TransformComponent_SetPosition);
    lua_setfield(state, -2, "SetPosition");

    lua_pushcfunction(state, TransformComponent_GetPosition);
    lua_setfield(state, -2, "GetPosition");

    // Register as a global variable.
    lua_setfield(state, LUA_GLOBALSINDEX, "TransformComponent");
}

//
// Component System
//

Game::ComponentSystem* ComponentSystem_Check(lua_State* state, int index)
{
    Assert(state != nullptr);

    // Get the userdata pointer.
    void* memory = luaL_checkudata(state, index, "ComponentSystem");
    auto* object = *reinterpret_cast<Game::ComponentSystem**>(memory);
    Assert(memory != nullptr && object != nullptr);

    return object;
}

int ComponentSystem_GetTransform(lua_State* state)
{
    Assert(state != nullptr);

    // Get arguments from the stack.
    auto* componentSystem = ComponentSystem_Check(state, 1);
    Game::EntityHandle* entity = EntityHandle_Check(state, 2);

    // Call the method.
    auto* transform = componentSystem->Lookup<Game::Components::Transform>(*entity);

    // Push the result.
    TransformComponent_Push(state, transform);

    return 1;
}

void ComponentSystem_Register(Lua::State& state, Context& context)
{
    Assert(state.IsValid());
    Assert(context.componentSystem != nullptr);

    // Create an userdata pointer.
    void* memory = lua_newuserdata(state, sizeof(Game::ComponentSystem*));
    auto** pointer = reinterpret_cast<Game::ComponentSystem**>(memory);
    *pointer = context.componentSystem;

    // Create and set the metatable.
    luaL_newmetatable(state, "ComponentSystem");

    lua_pushliteral(state, "__index");
    lua_pushvalue(state, -2);
    lua_rawset(state, -3);

    lua_pushcfunction(state, ComponentSystem_GetTransform);
    lua_setfield(state, -2, "GetTransform");

    lua_setmetatable(state, -2);

    // Register as a global variable.
    lua_setfield(state, LUA_GLOBALSINDEX, "ComponentSystem");
}

//
// Registering Method
//

bool Game::RegisterScriptBindings(Lua::State& state, Context& context)
{
    Assert(state.IsValid());

    // Create a stack guard.
    Lua::StackGuard guard(state);

    // Register classes.
    Vec2_Register(state, context);
    InputState_Register(state, context);
    EntityHandle_Register(state, context);
    TransformComponent_Register(state, context);
    ComponentSystem_Register(state, context);

    return true;
}
