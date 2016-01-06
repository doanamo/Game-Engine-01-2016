#pragma once

#include "Precompiled.hpp"

//
// Output
//

namespace Logger
{
    // Forward declarations.
    class Message;

    // Output interface.
    class Output
    {
    public:
        virtual void Write(const Logger::Message& message) = 0;
    };
}
