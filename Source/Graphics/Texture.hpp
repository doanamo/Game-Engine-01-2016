#pragma once

#include "Precompiled.hpp"
#include "System/Resource.hpp"

//
// Texture
//
//  Encapsulates an OpenGL texture surface.
//  Can also load images from PNG files.
//
//  Example usage:
//      Graphics::Texture texture;
//      texture.Load("Path/To/File");
//      
//      GLuint texture = texture.GetHandle();
//

namespace Graphics
{
    // Texture class.
    class Texture : public System::Resource
    {
    public:
        Texture(System::ResourceManager* resourceManager = nullptr);
        ~Texture();

        // Restores instance to it's original state.
        void Cleanup();

        // Loads the texture from a file.
        bool Load(std::string filename);

        // Initializes the texture instance.
        bool Initialize(int width, int height, GLenum format, const void* data);

        // Updates the texture data.
        void Update(const void* data);

        // Gets the texture handle.
        GLuint GetHandle() const
        {
            return m_handle;
        }

        // Gets the texture width.
        int GetWidth() const
        {
            return m_width;
        }

        // Gets the texture height.
        int GetHeight() const
        {
            return m_height;
        }

        // Checks if instance is valid.
        bool IsValid() const
        {
            return m_initialized;
        }

    private:
        // Texture handle.
        GLuint m_handle;

        // Texture parameters.
        int m_width;
        int m_height;
        GLenum m_format;

        // Initialization state.
        bool m_initialized;
    };
}
