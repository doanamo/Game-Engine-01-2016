#pragma once

#include "Precompiled.hpp"
#include "Game/EntityHandle.hpp"
#include "Game/Component.hpp"
#include "Lua/Lua.hpp"

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
            template<typename... Types, typename... Arguments>
            typename Lua::StackPopper<sizeof...(Types), Types...>::ReturnType Call(std::string method, Arguments&&... arguments);

        private:
            // List of script references.
            std::vector<Lua::Reference> m_scripts;
        };

        // Template definitions.
        template<typename... Types, typename... Arguments>
        typename Lua::StackPopper<sizeof...(Types), Types...>::ReturnType Script::Call(std::string method, Arguments&&... arguments)
        {
            for(auto& script : m_scripts)
            {
                // Get the scripting state.
                Lua::State& state = *script.GetState();
                Lua::StackGuard guard(&state);

                // Push a script instance on the stack.
                Lua::Push(state, script);

                // Call the script method.
                state.Call(method.c_str(), Lua::StackValue(-1), std::forward<Arguments>(arguments)...);
            }
        }
    }
}
