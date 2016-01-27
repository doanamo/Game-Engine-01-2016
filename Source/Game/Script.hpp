#pragma once

#include "Precompiled.hpp"
#include "System/Resource.hpp"
#include "Lua/State.hpp"

//
// Script
//
//  Resource which holds a reference to a script.
//  See ScriptSystem for more context.
//

namespace Game
{
    // Script class.
    class Script : public System::Resource
    {
    public:
        Script(System::ResourceManager* resourceManager);
        ~Script();

        // Restores instance to it's original state.
        void Cleanup();

        // Loads the script from a file.
        bool Load(std::string filename);

    private:
        // Script state reference.
        std::shared_ptr<Lua::State> m_state;

        // Script class reference.
        int m_reference;

        // Initialization state.
        bool m_initialized;
    };
}
