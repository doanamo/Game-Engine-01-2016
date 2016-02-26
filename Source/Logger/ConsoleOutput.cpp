#include "Precompiled.hpp"
#include "ConsoleOutput.hpp"
using namespace Logger;

ConsoleOutput::ConsoleOutput()
{
}

ConsoleOutput::~ConsoleOutput()
{
}

void ConsoleOutput::Write(const Logger::Message& message)
{
    // Write message prefix.
    time_t timeData = time(nullptr);
    tm* timeInfo = localtime(&timeData);

    std::cout << "[";
    std::cout << std::setfill('0');
    std::cout << std::setw(2) << timeInfo->tm_hour << ":";
    std::cout << std::setw(2) << timeInfo->tm_min  << ":";
    std::cout << std::setw(2) << timeInfo->tm_sec;
    std::cout << "] ";

    // Write message text.
    std::cout << message.GetText();

    // Write message source.
    if(!message.GetSource().empty())
    {
        std::cout << " {";
        std::cout << message.GetSource();

        if(message.GetLine() != 0)
        {
            std::cout << ":";
            std::cout << message.GetLine();
        }

        std::cout << "}";
    }

    // Write message suffix.
    std::cout << "\n";

    // Flush the file stream.
    std::cout.flush();
}
