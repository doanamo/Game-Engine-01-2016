#pragma once

#include "Precompiled.hpp"

//
// Sink
//

namespace Logger
{
    // Forward declarations.
    class Output;
    class Message;

    // Sink interface.
    class SinkBase
    {
    public:
        virtual void Write(const Logger::Message& message) = 0;
    };

    // Sink class.
    class Sink : public SinkBase, private NonCopyable
    {
    public:
        // Type declarations.
        typedef std::vector<Logger::Output*> OutputList;

    public:
        Sink();
        ~Sink();

        // Restores instance to it's original state.
        void Cleanup();

        // Adds an output.
        void AddOutput(Logger::Output* output);

        // Removes an output.
        void RemoveOutput(Logger::Output* output);

        // Writes a log message.
        void Write(const Logger::Message& message);

    private:
        // List of outputs.
        OutputList m_outputs;
    };
}
