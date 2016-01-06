#include "Precompiled.hpp"
#include "Utility.hpp"

std::vector<std::string> Utility::SplitString(std::string text, char character)
{
    std::vector<std::string> result;

    auto begin = text.begin();
    auto it = text.begin();

    while(it != text.end())
    {
        if(*it == character)
        {
            result.push_back(std::string(begin, it));
            begin = ++it;
        }
        else
        {
            ++it;
        }

        if(it == text.end())
        {
            result.push_back(std::string(begin, it));
        }
    }

    return result;
}

std::string Utility::GetFileExtension(std::string filename)
{
    std::string extension;

    size_t it = filename.rfind('.');
    if(it != std::string::npos)
    {
        extension = filename.substr(it + 1);
    }

    return extension;
}

std::string Utility::GetTextFileContent(std::string filename)
{
    std::ifstream file(filename);
    std::string content;

    if(file)
    {
        file.seekg(0, std::ios::end);
        content.resize((unsigned int) file.tellg());
        file.seekg(0, std::ios::beg);

        file.read(&content[0], content.size());
    }

    return content;
}

std::vector<char> Utility::GetBinaryFileContent(std::string filename)
{
    std::ifstream file(filename, std::ios::binary);
    std::vector<char> content;

    if(file)
    {
        file.seekg(0, std::ios::end);
        content.resize((unsigned int)file.tellg());
        file.seekg(0, std::ios::beg);

        file.read(&content[0], content.size());
    }

    return content;
}
