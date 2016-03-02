#include "Precompiled.hpp"
#include "Sampler.hpp"
using namespace Graphics;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a sampler! "

    // Invalid types.
    const GLuint InvalidHandle = 0;
}

Sampler::Sampler() :
    m_handle(InvalidHandle),
    m_initialized(false)
{
}

Sampler::~Sampler()
{
    this->Cleanup();
}

void Sampler::Cleanup()
{
    if(!m_initialized)
        return;

    // Delete sampler handle.
    if(m_handle != InvalidHandle)
    {
        glDeleteSamplers(1, &m_handle);
        m_handle = InvalidHandle;
    }

    // Reset initialization state.
    m_initialized = false;
}

bool Sampler::Initialize()
{
    this->Cleanup();

    // Setup a cleanup guard.
    SCOPE_GUARD
    (
        if(!m_initialized)
        {
            m_initialized = true;
            this->Cleanup();
        }
    );

    // Create a sampler handle.
    glGenSamplers(1, &m_handle);

    if(m_handle == InvalidHandle)
    {
        Log() << LogInitializeError() << "Couldn't create a sampler.";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void Sampler::SetParameter(GLenum parameter, GLint value)
{
    if(!m_initialized)
        return;

    glSamplerParameteri(m_handle, parameter, value);
}

GLuint Sampler::GetHandle() const
{
    return m_handle;
}
