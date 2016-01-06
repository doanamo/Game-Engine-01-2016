#pragma once

#include "Precompiled.hpp"
#include "Logger/Output.hpp"

//
// File Output
//

namespace Logger
{
    class FileOutput : public Logger::Output
    {
    public:
        FileOutput();
        ~FileOutput();

        // Restores instance to it's original state.
        void Cleanup();
        
        // Initializes the file output.
        bool Initialize(std::string filename);

        // Writes a message to file.
        void Write(const Logger::Message& message);

    private:
        // File output.
        std::ofstream m_file;

        // Initialization state.
        bool m_initialized;
    };
}
