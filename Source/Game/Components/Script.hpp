#pragma once

#include "Precompiled.hpp"
#include "Game/EntityHandle.hpp"
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
            Script();
            ~Script();

            // Adds a script instance.
            void AddScript(std::shared_ptr<const Lua::Reference> script);

            // Finalizes the component.
            bool Finalize(EntityHandle self, const Context& context);

            // Calls added scripts.
            void Call(std::string function);

        private:
            // List of script references.
            std::vector<Lua::Reference> m_scripts;
        };
    }
}
