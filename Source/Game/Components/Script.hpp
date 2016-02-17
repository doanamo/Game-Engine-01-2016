#pragma once

#include "Precompiled.hpp"
#include "Game/Component.hpp"
#include "Lua/State.hpp"
#include "Lua/Reference.hpp"

//
// Script Component
//

namespace Game
{
    namespace Components
    {
        // Script component class.
        class Script : public Game::Component
        {
        public:
            // Type decalarations.
            typedef int ReferenceID;

        public:
            Script();
            ~Script();

            // Adds a script instance.
            void AddScript(std::shared_ptr<const Lua::Reference> script);

            // Calls added scripts.
            void Call(std::string function);

        private:
            // Script state reference.
            std::shared_ptr<Lua::State> m_state;

            // List of script references.
            std::vector<ReferenceID> m_scripts;
        };
    }
}
