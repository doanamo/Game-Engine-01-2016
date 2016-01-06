#pragma once

#include "Precompiled.hpp"
#include "Logger/Output.hpp"

//
// Debugger Output
//

namespace Logger
{
    class DebuggerOutput : public Logger::Output
    {
    public:
        DebuggerOutput();
        ~DebuggerOutput();

        // Writes a message to debugger window.
        void Write(const Logger::Message& message);

    private:
        std::stringstream m_stream;
    };
}
