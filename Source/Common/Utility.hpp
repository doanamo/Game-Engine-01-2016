#pragma once

#include "Precompiled.hpp"

//
// Utility
//

namespace Utility
{
    // Clamps a value between the lower and upper range.
    template<typename Type>
    Type Clamp(const Type& value, const Type& lower, const Type& upper)
    {
        return std::max(lower, std::min(value, upper));
    }

    // Gets the size of a static array.
    template<typename Type, size_t Size>
    size_t ArraySize(const Type(&)[Size])
    {
        return Size;
    }

    // Frees the container's memory.
    template<typename Type>
    void ClearContainer(Type& container)
    {
        container.swap(Type());
    }

    // Reorders a vector using given indices.
    template<typename Type>
    void Reorder(std::vector<Type>& values, const std::vector<std::size_t>& order)
    {
        Assert(values.size() == order.size());

        // Create an array of indices.
        std::vector<std::size_t> indices(order.size());
        std::iota(indices.begin(), indices.end(), 0);

        // Rearange the values.
        for(std::size_t i = 0; i < values.size() - 1; ++i)
        {
            std::size_t desired = order[i];

            for(std::size_t j = i; j < values.size(); ++j)
            {
                if(desired == indices[j])
                {
                    std::swap(values[i], values[j]);
                    std::swap(indices[i], indices[j]);
                    break;
                }
            }
        }
    }

    // Splits a string into tokens.
    std::vector<std::string> SplitString(std::string text, char character = ' ');
    
    // Gets the path of a file.
    std::string GetFilePath(std::string filename);

    // Gets the extension of a file.
    std::string GetFileExtension(std::string filename);

    // Gets the content of a text file.
    std::string GetTextFileContent(std::string filename);

    // Gets the content of a binary file.
    std::vector<char> GetBinaryFileContent(std::string filename);
}
