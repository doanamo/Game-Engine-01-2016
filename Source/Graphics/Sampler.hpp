#pragma once

#include "Precompiled.hpp"

//
// Sampler
//
//  Encapsulates an OpenGL sampler object.
//
//  Example usage:
//      Graphics::Sampler sampler;
//      sampler.Initialize();
//      
//      sampler.SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//      sampler.SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//      GLuint handle = sampler.GetHandle();
//

namespace Graphics
{
    // Sampler class.
    class Sampler
    {
    public:
        Sampler();
        ~Sampler();

        // Restore instance to it's original state.
        void Cleanup();

        // Initializes the sampler object.
        bool Initialize();

        // Sets a sampler parameter.
        void SetParameter(GLenum parameter, GLint value);

        // Gets the sampler handle.
        GLuint GetHandle() const;

    private:
        // Sampler handle.
        GLuint m_handle;

        // Initialization state.
        bool m_initialized;
    };
}
