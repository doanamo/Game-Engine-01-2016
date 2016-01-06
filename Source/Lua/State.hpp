#pragma once

#include "Precompiled.hpp"

//
// Lua State
//

namespace Lua
{
    // State class.
    class State
    {
    public:
        State();
        ~State();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the Lua state.
        bool Initialize();

        // Loads a script file.
        bool Load(std::string filename);
        
        // Conversion operator.
        operator lua_State*();

        // Prints stack trace for debugging.
        void PrintStack() const;

    private:
        // Virtual machine state.
        lua_State* m_state;

        // Initialization state.
        bool m_initialized;
    };
}
