#pragma once

#include "Precompiled.hpp"
#include "Lua/State.hpp"

namespace Game
{
    class EntitySystem;
    class ComponentSystem;
}

//
// Script System
//
//  Manages entity logic via script components.
//

namespace Game
{
    // Script system class.
    class ScriptSystem
    {
    public:
        ScriptSystem();
        ~ScriptSystem();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the script system.
        bool Initialize(Context& context);

        // Updates the system.
        void Update(float timeDelta);

        // Gets the Lua state.
        std::shared_ptr<Lua::State> GetState();

    private:
        // Context references.
        EntitySystem*    m_entitySystem;
        ComponentSystem* m_componentSystem;

        // Main scripting state.
        std::shared_ptr<Lua::State> m_state;

        // Initialization state.
        bool m_initialized;
    };
}
