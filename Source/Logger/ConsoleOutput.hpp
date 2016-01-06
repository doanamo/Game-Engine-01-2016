#pragma once

#include "Precompiled.hpp"
#include "Logger/Output.hpp"

//
// Console Output
//

namespace Logger
{
    class ConsoleOutput : public Logger::Output
    {
    public:
        ConsoleOutput();
        ~ConsoleOutput();

        // Writes a message to the console.
        void Write(const Logger::Message& message);
    };
}
