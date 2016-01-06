#include "Precompiled.hpp"
#include "Utility.hpp"

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
