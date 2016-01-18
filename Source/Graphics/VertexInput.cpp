#include "Precompiled.hpp"
#include "VertexInput.hpp"
#include "Buffer.hpp"
using namespace Graphics;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a vertex input! "

    // Gets the vertex attribute type row size.
    int GetVertexAttributeTypeRowSize(VertexAttributeTypes type)
    {
        switch(type)
        {
            case VertexAttributeTypes::Float1:
                return 1;

            case VertexAttributeTypes::Float2:
                return 2;

            case VertexAttributeTypes::Float3:
                return 3;

            case VertexAttributeTypes::Float4:
                return 4;

            case VertexAttributeTypes::Float4x4:
                return 4;
        }

        return 0;
    }

    // Gets the vertex attribute type row count.
    int GetVertexAttributeTypeRowCount(VertexAttributeTypes type)
    {
        switch(type)
        {
            case VertexAttributeTypes::Float1:
            case VertexAttributeTypes::Float2:
            case VertexAttributeTypes::Float3:
            case VertexAttributeTypes::Float4:
                return 1;

            case VertexAttributeTypes::Float4x4:
                return 4;
        }

        return 0;
    }

    // Gets the vertex attribute type row offset.
    int GetVertexAttributeTypeRowOffset(VertexAttributeTypes type)
    {
        switch(type)
        {
            case VertexAttributeTypes::Float1:
                return sizeof(float) * 1;

            case VertexAttributeTypes::Float2:
                return sizeof(float) * 2;

            case VertexAttributeTypes::Float3:
                return sizeof(float) * 3;

            case VertexAttributeTypes::Float4:
                return sizeof(float) * 4;

            case VertexAttributeTypes::Float4x4:
                return sizeof(float) * 4;
        }

        return 0;
    }

    // Gets the vertex attribute type enum.
    GLenum GetVertexAttributeTypeEnum(VertexAttributeTypes type)
    {
        switch(type)
        {
            case VertexAttributeTypes::Float1:
            case VertexAttributeTypes::Float2:
            case VertexAttributeTypes::Float3:
            case VertexAttributeTypes::Float4:
            case VertexAttributeTypes::Float4x4:
                return GL_FLOAT;
        }

        return GL_INVALID_ENUM;
    }

    // Constant definitions.
    const GLuint InvalidHandle = 0;
}

VertexInput::VertexInput() :
    m_handle(InvalidHandle),
    m_initialized(false)
{
}

VertexInput::~VertexInput()
{
    if(m_initialized)
        this->Cleanup();
}

void VertexInput::Cleanup()
{
    // Release the vertex array handle.
    if(m_handle != InvalidHandle)
    {
        glDeleteVertexArrays(1, &m_handle);
        m_handle = InvalidHandle;
    }

    // Reset initialization state.
    m_initialized = false;
}

bool VertexInput::Initialize(int attributeCount, const VertexAttribute* attributes)
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
    if(attributeCount <= 0)
    {
        Log() << LogInitializeError() << "Invalid argument - \"count\" is 0.";
        return false;
    }

    if(attributes == nullptr)
    {
        Log() << LogInitializeError() << "Invalid argument - \"attributes\" is null.";
        return false;
    }

    for(int i = 0; i < attributeCount; ++i)
    {
        const VertexAttribute& attribute = attributes[i];

        if(attribute.buffer == nullptr)
        {
            Log() << LogInitializeError() << "Invalid argument - \"attribute[" << i << "].buffer\" is null.";
            return false;
        }

        if(!attribute.buffer->IsValid())
        {
            Log() << LogInitializeError() << "Invalid argument - \"attribute[" << i << "].buffer\" is invalid.";
            return false;
        }

        if(attribute.buffer->GetType() != GL_ARRAY_BUFFER)
        {
            Log() << LogInitializeError() << "Invalid argument - \"attribute[" << i << "].buffer\" is not a vertex or an instance buffer.";
            return false;
        }

        if(attribute.type == VertexAttributeTypes::Invalid)
        {
            Log() << LogInitializeError() << "Invalid argument - \"attribute[" << i << "].type\" is invalid.";
            return false;
        }
    }

    // Create a vertex array object.
    glGenVertexArrays(1, &m_handle);

    if(m_handle == 0)
    {
        Log() << LogInitializeError() << "Couldn't create a vertex array object.";
        return false;
    }

    // Cleanup state after we are done.
    SCOPE_GUARD
    (
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    );

    // Bind the vertex array.
    glBindVertexArray(m_handle);

    // Set the vertex array state.
    const Buffer* currentBuffer = nullptr;

    int currentLocation = 0;
    int currentOffset = 0;

    for(int i = 0; i < attributeCount; ++i)
    {
        const VertexAttribute& attribute = attributes[i];

        // Bind the vertex buffer.
        if(currentBuffer != attribute.buffer)
        {
            glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer->GetHandle());

            currentBuffer = attribute.buffer;
            currentOffset = 0;
        }

        // Setup vertex attributes for each used vertex location.
        for(int l = 0; l < GetVertexAttributeTypeRowCount(attribute.type); ++l)
        {
            // Enable vertex attribute.
            glEnableVertexAttribArray(currentLocation);

            // Set vertex attribute pointer.
            glVertexAttribPointer(
                currentLocation,
                GetVertexAttributeTypeRowSize(attribute.type),
                GetVertexAttributeTypeEnum(attribute.type),
                GL_FALSE,
                attribute.buffer->GetElementSize(),
                (void*)currentOffset
            );

            // Make vertex location instanced.
            if(attribute.buffer->IsInstanced())
            {
                glVertexAttribDivisor(currentLocation, 1);
            }

            // Increment current location.
            currentLocation += 1;

            // Increment current offset.
            currentOffset += GetVertexAttributeTypeRowOffset(attribute.type);
        }
    }

    // Success!
    return m_initialized = true;
}
