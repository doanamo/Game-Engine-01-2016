#pragma once

#include "Precompiled.hpp"
#include "System/Resource.hpp"

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
        Script(System::ResourceManager* resourceManager = nullptr);
        ~Script();

        // Restores instance to it's original state.
        void Cleanup();

        // Loads the script from a file.
        bool Load(std::string filename);

    private:
        // Initialization state.
        bool m_initialized;
    };
}
