#include "Precompiled.hpp"
#include "Logger.hpp"
#include "Sink.hpp"
#include "FileOutput.hpp"
#include "ConsoleOutput.hpp"
#include "DebuggerOutput.hpp"

namespace
{
    // Logger sink.
    Logger::Sink sink;

    // Logger outputs.
    Logger::FileOutput fileOutput;
    Logger::ConsoleOutput consoleOutput;
    Logger::DebuggerOutput debuggerOutput;
}

void Logger::Initialize()
{
    // Add the file output.
    if(fileOutput.Initialize("Log.txt"))
    {
        sink.AddOutput(&fileOutput);
    }

    // Add the console output.
    sink.AddOutput(&consoleOutput);

    // Add the debugger output.
    sink.AddOutput(&debuggerOutput);
}

void Logger::Write(const Logger::Message& message)
{
    sink.Write(message);
}

Logger::SinkBase* Logger::GetGlobal()
{
    return &sink;
}
