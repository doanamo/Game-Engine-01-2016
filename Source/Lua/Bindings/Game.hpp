#pragma once

#include "Precompiled.hpp"
#include "Context.hpp"
#include "Lua/Lua.hpp"

// Forward declarations.
namespace Game
{
    struct EntityHandle;
    class ComponentSystem;

    namespace Components
    {
        class Transform;
    }
}

//
// Entity Handle
//

namespace Lua
{
    namespace Bindings
    {
        namespace EntityHandle
        {
            // Helper functions.
            Game::EntityHandle* Push(lua_State* state);
            Game::EntityHandle* Check(lua_State* state, int index);

            // Meta methods.
            int New(lua_State* state);
            int Index(lua_State* state);
            int NewIndex(lua_State* state);

            // Registers Lua bindings.
            void Register(Lua::State& state, Context& context);
        }
    }
}

//
// Component System
//

namespace Lua
{
    namespace Bindings
    {
        namespace ComponentSystem
        {
            // Helper functions.
            Game::ComponentSystem* Check(lua_State* state, int index);

            // Class methods.
            int GetTransform(lua_State* state);

            // Registers Lua bindings.
            void Register(Lua::State& state, Context& context);
        }
    }
}

//
// Transform Component
//

namespace Lua
{
    namespace Bindings
    {
        namespace TransformComponent
        {
            // Helper functions.
            Game::Components::Transform* Push(lua_State* state, Game::Components::Transform* transform);
            Game::Components::Transform* Check(lua_State* state, int index);

            // Class methods.
            int SetPosition(lua_State* state);
            int GetPosition(lua_State* state);

            // Registers Lua bindings.
            void Register(Lua::State& state, Context& context);
        }
    }
}
