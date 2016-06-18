#include "Precompiled.hpp"
#include "Game.hpp"
#include "Math.hpp"
using namespace Lua;
using namespace Bindings;

#include "Game/EntityHandle.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Components/Transform.hpp"

//
// Entity Handle
//

Game::EntityHandle* EntityHandle::Push(lua_State* state)
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

Game::EntityHandle* EntityHandle::Check(lua_State* state, int index)
{
    Assert(state != nullptr);

    // Get the userdata object.
    void* memory = luaL_checkudata(state, index, "EntityHandle");
    auto* object = reinterpret_cast<Game::EntityHandle*>(memory);
    Assert(memory != nullptr && object != nullptr);

    return object;
}

int EntityHandle::New(lua_State* state)
{
    Assert(state != nullptr);

    // Push the instance.
    EntityHandle::Push(state);

    return 1;
}

int EntityHandle::Index(lua_State* state)
{
    Assert(state != nullptr);

    // Get arguments from the stack.
    auto* handle = EntityHandle::Check(state, 1);
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

int EntityHandle::NewIndex(lua_State* state)
{
    Assert(state != nullptr);

    // Get arguments from the stack.
    auto* handle = EntityHandle::Check(state, 1);
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

void EntityHandle::Register(Lua::State& state, Context& context)
{
    Assert(state.IsValid());

    // Create a class metatable.
    luaL_newmetatable(state, "EntityHandle");

    lua_pushcfunction(state, EntityHandle::New);
    lua_setfield(state, -2, "New");

    lua_pushcfunction(state, EntityHandle::Index);
    lua_setfield(state, -2, "__index");

    lua_pushcfunction(state, EntityHandle::NewIndex);
    lua_setfield(state, -2, "__newindex");

    // Create a secondary table.
    lua_newtable(state);

    lua_pushcfunction(state, EntityHandle::New);
    lua_setfield(state, -2, "__call");

    lua_setmetatable(state, -2);

    // Register as a global variable.
    lua_setfield(state, LUA_GLOBALSINDEX, "EntityHandle");
}

//
// Component System
//

Game::ComponentSystem* ComponentSystem::Check(lua_State* state, int index)
{
    Assert(state != nullptr);

    // Get the userdata pointer.
    void* memory = luaL_checkudata(state, index, "ComponentSystem");
    auto* object = *reinterpret_cast<Game::ComponentSystem**>(memory);
    Assert(memory != nullptr && object != nullptr);

    return object;
}

int ComponentSystem::GetTransform(lua_State* state)
{
    Assert(state != nullptr);

    // Get arguments from the stack.
    auto* componentSystem = ComponentSystem::Check(state, 1);
    Game::EntityHandle* entity = EntityHandle::Check(state, 2);

    // Call the method.
    auto* transform = componentSystem->Lookup<Game::Components::Transform>(*entity);

    // Push the result.
    TransformComponent::Push(state, transform);

    return 1;
}

void ComponentSystem::Register(Lua::State& state, Context& context)
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

    lua_pushcfunction(state, ComponentSystem::GetTransform);
    lua_setfield(state, -2, "GetTransform");

    lua_setmetatable(state, -2);

    // Register as a global variable.
    lua_setfield(state, LUA_GLOBALSINDEX, "ComponentSystem");
}

//
// Transform Component
//

Game::Components::Transform* TransformComponent::Push(lua_State* state, Game::Components::Transform* transform)
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

Game::Components::Transform* TransformComponent::Check(lua_State* state, int index)
{
    Assert(state != nullptr);

    // Get the userdata pointer.
    void* memory = luaL_checkudata(state, index, "TransformComponent");
    auto* object = *reinterpret_cast<Game::Components::Transform**>(memory);
    Assert(memory != nullptr && object != nullptr);

    return object;
}

int TransformComponent::SetPosition(lua_State* state)
{
    Assert(state != nullptr);

    // Get arguments from the stack.
    auto* transform = TransformComponent::Check(state, 1);
    glm::vec2* position = Vec2::Check(state, 2);

    // Call the method.
    transform->SetPosition(*position);

    return 0;
}

int TransformComponent::GetPosition(lua_State* state)
{
    Assert(state != nullptr);

    // Get arguments from the stack.
    auto* transform = TransformComponent::Check(state, 1);

    // Call the method.
    glm::vec2 position = transform->GetPosition();

    // Push the result.
    *Vec2::Push(state) = position;

    return 1;
}

void TransformComponent::Register(Lua::State& state, Context& context)
{
    Assert(state.IsValid());

    // Create a class metatable.
    luaL_newmetatable(state, "TransformComponent");

    lua_pushliteral(state, "__index");
    lua_pushvalue(state, -2);
    lua_rawset(state, -3);

    lua_pushcfunction(state, TransformComponent::SetPosition);
    lua_setfield(state, -2, "SetPosition");

    lua_pushcfunction(state, TransformComponent::GetPosition);
    lua_setfield(state, -2, "GetPosition");

    // Register as a global variable.
    lua_setfield(state, LUA_GLOBALSINDEX, "TransformComponent");
}
