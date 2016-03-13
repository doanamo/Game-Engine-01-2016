#pragma once

#include "Precompiled.hpp"
#include "System/Resource.hpp"

//
// Reference
//
//  Object which holds a reference to Lua object.
//  Can be also used as a resource bound to ScriptSystem.
//

namespace Lua
{
    // Forward declarations.
    class State;

    // Reference class.
    class Reference : public System::Resource
    {
    public:
        // Type declarations.
        typedef int ReferenceID;

    public:
        Reference(System::ResourceManager* resourceManager);
        Reference(const std::shared_ptr<Lua::State>& state);
        Reference(const Reference& other);
        Reference(Reference&& other);
        ~Reference();

        // Releases the referenced value.
        void Release();

        // Loads the reference from a file.
        bool Load(std::string filename);

        // Creates a reference for an object on top of the stack.
        // Pops the object from the stack in the process.
        void CreateFromStack();

        // Pushes the referenced value on top of the stack.
        void PushOntoStack() const;

        // Gets the hosting Lua state.
        std::shared_ptr<Lua::State> GetState() const;

        // Gets the reference identificator.
        ReferenceID GetReference() const;

        // Checks if the reference is valid.
        bool IsValid() const;

        // Comparison operators.
        bool operator==(const std::nullptr_t) const;
        bool operator!=(const std::nullptr_t) const;

    private:
        // Lua state reference.
        std::shared_ptr<Lua::State> m_state;

        // Lua value reference.
        ReferenceID m_reference;
    };
}
