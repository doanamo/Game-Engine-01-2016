#include "Precompiled.hpp"
#include "DebuggerOutput.hpp"
using namespace Logger;

DebuggerOutput::DebuggerOutput()
{
}

DebuggerOutput::~DebuggerOutput()
{
}

void DebuggerOutput::Write(const Logger::Message& message)
{
    // Don't do anything if not in debug mode.
    #ifdef NDEBUG
        return;
    #endif

    // Clear string stream.
    m_stream.str("");

    // Write message prefix.
    time_t timeData = time(nullptr);
    tm* timeInfo = localtime(&timeData);

    m_stream << "[";
    m_stream << std::setfill('0');
    m_stream << std::setw(2) << timeInfo->tm_hour << ":";
    m_stream << std::setw(2) << timeInfo->tm_min  << ":";
    m_stream << std::setw(2) << timeInfo->tm_sec;
    m_stream << "] ";

    // Write message text.
    m_stream << message.GetText();

    // Write message source.
    #ifndef NDEBUG
        if(!message.GetSource().empty())
        {
            m_stream << " {";
            m_stream << message.GetSource();

            if(message.GetLine() != 0)
            {
                m_stream << ":";
                m_stream << message.GetLine();
            }

            m_stream << "}";
        }
    #endif

    // Write message suffix.
    m_stream << "\n";

    // Output message to the debugger.
    #ifdef WIN32
        OutputDebugStringA(m_stream.str().c_str());
    #endif
}
