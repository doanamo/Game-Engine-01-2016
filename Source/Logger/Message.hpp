#pragma once

#include "Precompiled.hpp"

//
// Message
//

namespace Logger
{
    // Message class.
    class Message : private NonCopyable, public std::ostream
    {
    public:
        Message();
        Message(Message&& other);
        virtual ~Message();

        // Sets the message text.
        Message& SetText(const char* text);

        // Sets the message source.
        Message& SetSource(const char* source);

        // Sets the message line.
        Message& SetLine(int line);

        // Gets the message text.
        std::string GetText() const;

        // Gets the message source.
        std::string GetSource() const;

        // Gets the message line.
        int GetLine() const;

        // Checks if the message is empty.
        bool IsEmpty() const;

    private:
        // Message state.
        std::stringbuf m_text;
        std::string    m_source;
        int            m_line;
    };
}

//
// Scoped Message
//

namespace Logger
{
    // Forward declarations.
    class SinkBase;

    // Scoped message class.
    class ScopedMessage : public Message
    {
    public:
        ScopedMessage(Logger::SinkBase* sink);
        ScopedMessage(ScopedMessage&& other);
        ~ScopedMessage();

    private:
        // Message sink output.
        Logger::SinkBase* m_sink;
    };
}
