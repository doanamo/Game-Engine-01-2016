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
        // Type declarations.
        typedef int ScriptReference;

    public:
        Script(System::ResourceManager* resourceManager);
        ~Script();

        // Restores instance to it's original state.
        void Cleanup();

        // Loads the script from a file.
        bool Load(std::string filename);

        // Gets the scripting state.
        std::shared_ptr<Lua::State> GetState() const;

        // Gets the script reference.
        ScriptReference GetReference() const;

    private:
        // Scripting state.
        std::shared_ptr<Lua::State> m_state;

        // Script reference.
        ScriptReference m_reference;

        // Initialization state.
        bool m_initialized;
    };
}
