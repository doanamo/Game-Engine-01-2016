#pragma once

#include "Precompiled.hpp"

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

    private:
        // Initialization state.
        bool m_initialized;
    };
}
