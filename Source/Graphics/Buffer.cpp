#include "Precompiled.hpp"
#include "Buffer.hpp"
using namespace Graphics;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a buffer! "

    // Constant definitions.
    const GLuint InvalidHandle = 0;
    const GLenum InvalidEnum = 0;
}

Buffer::Buffer(GLenum type) :
    m_type(type),
    m_handle(InvalidHandle),
    m_elementSize(0),
    m_elementCount(0),
    m_initialized(false)
{
}

Buffer::~Buffer()
{
    if(m_initialized)
        this->Cleanup();
}

void Buffer::Cleanup()
{
    // Release the buffer handle.
    if(m_handle != InvalidHandle)
    {
        glDeleteBuffers(1, &m_handle);
        m_handle = InvalidHandle;
    }

    // Reset buffer parameters.
    m_elementSize = 0;
    m_elementCount = 0;

    // Reset initialization state.
    m_initialized = false;
}

bool Buffer::Initialize(unsigned int elementSize, unsigned int elementCount, const void* data, GLenum usage)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Validate arguments.
    if(elementSize == 0)
    {
        Log() << LogInitializeError() << "Invalid argument - \"elementSize\" is 0.";
        return false;
    }

    if(elementCount == 0)
    {
        Log() << LogInitializeError() << "Invalid argument - \"elementCount\" is 0.";
        return false;
    }

    m_elementSize = elementSize;
    m_elementCount = elementCount;

    // Create a buffer.
    glGenBuffers(1, &m_handle);

    if(m_handle == InvalidHandle)
    {
        Log() << LogInitializeError() << "Couldn't create a buffer.";
        return false;
    }

    // Copy data to the buffer.
    unsigned int bufferSize = m_elementSize * m_elementCount;

    glBindBuffer(m_type, m_handle);
    glBufferData(m_type, bufferSize, data, usage);
    glBindBuffer(m_type, 0);

    // Success!
    Log() << "Created " << this->GetName() << " (" << bufferSize << " bytes).";

    return m_initialized = true;
}

void Buffer::Update(const void* data, int count)
{
    if(!m_initialized)
        return;

    // Validate arguments.
    if(data == nullptr)
        return;

    if(count == 0)
        return;

    // Check if to upload the whole buffer.
    if(count < 0)
    {
        count = m_elementCount;
    }

    // Upload new buffer data.
    glBindBuffer(m_type, m_handle);
    glBufferSubData(m_type, 0, m_elementSize * count, data);
    glBindBuffer(m_type, 0);
}

GLenum IndexBuffer::GetElementType() const
{
    if(!m_initialized)
        return InvalidEnum;

    if(m_type == GL_ELEMENT_ARRAY_BUFFER)
    {
        switch(m_elementSize)
        {
            case 1: return GL_UNSIGNED_BYTE;
            case 2: return GL_UNSIGNED_SHORT;
            case 4: return GL_UNSIGNED_INT;
        }
    }

    return InvalidEnum;
}
