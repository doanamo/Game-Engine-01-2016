#pragma once

#include "Precompiled.hpp"

//
// Utility
//

namespace Utility
{
    // Gets the content of a text file.
    std::string GetTextFileContent(std::string filename);

    // Gets the content of a binary file.
    std::vector<char> GetBinaryFileContent(std::string filename);
}
