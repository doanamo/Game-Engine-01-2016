#include "Precompiled.hpp"
#include "FileOutput.hpp"
using namespace Logger;

FileOutput::FileOutput() :
    m_initialized(false)
{
}

FileOutput::~FileOutput()
{
    if(m_initialized)
        this->Cleanup();
}

void FileOutput::Cleanup()
{
    // Close the file.
    if(m_file.is_open())
    {
        // Write session end.
        time_t timeData = time(nullptr);
        tm* timeInfo = localtime(&timeData);

        m_file << "\n";
        m_file << "Session ended at ";
        m_file << std::setfill('0');
        m_file << std::setw(4) << timeInfo->tm_year + 1900 << "-";
        m_file << std::setw(2) << timeInfo->tm_mon + 1     << "-";
        m_file << std::setw(2) << timeInfo->tm_mday        << " ";
        m_file << std::setw(2) << timeInfo->tm_hour        << ":";
        m_file << std::setw(2) << timeInfo->tm_min         << ":";
        m_file << std::setw(2) << timeInfo->tm_sec;

        m_file.flush();

        // Close the file.
        m_file.close();
    }

    // Reset initialization state.
    m_initialized = false;
}
        
bool FileOutput::Initialize(std::string filename)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Open the file for write.
    m_file.open(filename);

    if(!m_file.is_open())
        return false;

    // Write session start.
    time_t timeData = time(nullptr);
    tm* timeInfo = localtime(&timeData);

    m_file << "Session started at ";
    m_file << std::setfill('0');
    m_file << std::setw(4) << timeInfo->tm_year + 1900 << "-";
    m_file << std::setw(2) << timeInfo->tm_mon + 1     << "-";
    m_file << std::setw(2) << timeInfo->tm_mday        << " ";
    m_file << std::setw(2) << timeInfo->tm_hour        << ":";
    m_file << std::setw(2) << timeInfo->tm_min         << ":";
    m_file << std::setw(2) << timeInfo->tm_sec;
    m_file << "\n\n";

    m_file.flush();

    // Success!
    return m_initialized = true;
}

void FileOutput::Write(const Logger::Message& message)
{
    if(!m_initialized)
        return;

    // Write message prefix.
    time_t timeData = time(nullptr);
    tm* timeInfo = localtime(&timeData);

    m_file << "[";
    m_file << std::setfill('0');
    m_file << std::setw(2) << timeInfo->tm_hour << ":";
    m_file << std::setw(2) << timeInfo->tm_min  << ":";
    m_file << std::setw(2) << timeInfo->tm_sec;
    m_file << "] ";

    // Write message text.
    m_file << message.GetText();

    // Write message source.
    #ifndef NDEBUG
        if(!message.GetSource().empty())
        {
            m_file << " {";
            m_file << message.GetSource();

            if(message.GetLine() != 0)
            {
                m_file << ":";
                m_file << message.GetLine();
            }

            m_file << "}";
        }
    #endif

    // Write message suffix.
    m_file << "\n";

    // Flush the file stream.
    m_file.flush();
}
