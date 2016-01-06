#include "Precompiled.hpp"
#include "Message.hpp"
#include "Sink.hpp"
using namespace Logger;

Message::Message() :
    std::ostream(&m_text),
    m_source(),
    m_line(0)
{
}

Message::Message(Message&& other) :
    std::ostream(&m_text)
{
    m_text = std::move(other.m_text);
    m_source = std::move(other.m_source);

    m_line = other.m_line;
    other.m_line = 0;
}

Message::~Message()
{
}

Message& Message::SetText(const char* text)
{
    if(text != nullptr)
    {
        m_text.str(text);
    }
    else
    {
        m_text.str("");
    }

    return *this;
}

Message& Message::SetSource(const char* source)
{
    if(source != nullptr)
    {
        // Set source string.
        m_source = source;

        // Normalize source path separators.
        std::replace(m_source.begin(), m_source.end(), '\\', '/');

        // Remove base path to source directory.
        std::string sourceDir = Build::GetSourceDir();

        auto it = std::search(m_source.begin(), m_source.end(), sourceDir.begin(), sourceDir.end(), [](char a, char b)
        {
            return std::toupper(a) == std::toupper(b);
        });

        if(it != m_source.end())
        {
            m_source.erase(it, it + sourceDir.size());
        }
    }

    return *this;
}

Message& Message::SetLine(int line)
{
    if(line > 0)
    {
        m_line = line;
    }

    return *this;
}

std::string Message::GetText() const
{
    return m_text.str();
}

std::string Message::GetSource() const
{
    return m_source;
}

int Message::GetLine() const
{
    return m_line;
}

bool Message::IsEmpty() const
{
    return m_text.str().empty();
}

ScopedMessage::ScopedMessage(Logger::SinkBase* sink) :
    m_sink(sink)
{
}

ScopedMessage::ScopedMessage(ScopedMessage&& other)
{
    m_sink = other.m_sink;
    other.m_sink = nullptr;
}

ScopedMessage::~ScopedMessage()
{
    if(m_sink != nullptr)
    {
        m_sink->Write(*this);
    }
}