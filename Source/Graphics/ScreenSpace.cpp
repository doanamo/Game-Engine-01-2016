#include "Precompiled.hpp"
#include "ScreenSpace.hpp"
using namespace Graphics;

namespace
{
    // Floors a value and makes it a multiple of 2.
    template<typename Type>
    float FloorMultipleTwo(Type value)
    {
        int floor = (int)value;
        if(floor % 2) floor += 1;
        return (float)floor;
    }
}

ScreenSpace::ScreenSpace() :
    m_sourceSize(4.0f, 4.0f),
    m_targetSize(4.0f, 4.0f),
    m_targetAspect(1.0f),
    m_rectangle(0.0f, 0.0f, 0.0f, 0.0f),
    m_offset(0.0f, 0.0f),
    m_projection(1.0f),
    m_view(1.0f),
    m_transform(1.0f),
    m_rebuild(true),
    m_rebuildTarget(false)
{
}

ScreenSpace::~ScreenSpace()
{
}

void ScreenSpace::Cleanup()
{
    *this = ScreenSpace();
}

void ScreenSpace::SetSourceSize(int width, int height)
{
    // Floor and make a multiple of 2.
    m_sourceSize.x = FloorMultipleTwo(width);
    m_sourceSize.y = FloorMultipleTwo(height);

    m_rebuild = true;
}

void ScreenSpace::SetTargetSize(float width, float height)
{
    m_targetSize.x = width;
    m_targetSize.y = height;
    m_rebuildTarget = false;

    m_rebuild = true;
}

void ScreenSpace::SetTargetSizeAspect(float aspect)
{
    m_targetAspect = aspect;
    m_rebuildTarget = true;

    m_rebuild = true;
}

void ScreenSpace::Rebuild() const
{
    if(m_rebuild)
    {
        // Build target size if requested.
        if(m_rebuildTarget)
        {
            // Set target size equal to source.
            m_targetSize = m_sourceSize;

            // Adjust target size aspect ratio.
            float sourceAspect = m_sourceSize.x / m_sourceSize.y;

            if(sourceAspect > m_targetAspect)
            {
                m_targetSize.x = m_sourceSize.x / (sourceAspect / m_targetAspect);
            }
            else
            {
                m_targetSize.y = m_sourceSize.y * (sourceAspect / m_targetAspect);
            }

            // Floor and make a multiple of 2.
            m_targetSize.x = FloorMultipleTwo(m_targetSize.x);
            m_targetSize.y = FloorMultipleTwo(m_targetSize.y);
        }

        // Calculate apsect ratios.
        float sourceAspectRatio = m_sourceSize.x / m_sourceSize.y;
        float targetAspectRatio = m_targetSize.x / m_targetSize.y;

        float aspectRatio = sourceAspectRatio / targetAspectRatio;

        // Calculate screen space coordinates.
        m_rectangle.x = -m_targetSize.x * 0.5f; // Left
        m_rectangle.y =  m_targetSize.x * 0.5f; // Right
        m_rectangle.z = -m_targetSize.y * 0.5f; // Bottom
        m_rectangle.w =  m_targetSize.y * 0.5f; // Top

        if(sourceAspectRatio > targetAspectRatio)
        {
            // Scale screen space coordinates.
            m_rectangle.x *= aspectRatio;
            m_rectangle.y *= aspectRatio;
        }
        else
        {
            // Scale screen space coordinates.
            m_rectangle.z /= aspectRatio;
            m_rectangle.w /= aspectRatio;
        }

        // Calculate screen space offset.
        m_offset.x = -m_targetSize.x * 0.5f;
        m_offset.y = -m_targetSize.y * 0.5f;

        // Calculate screen space matrices.
        m_projection = glm::ortho(m_rectangle.x, m_rectangle.y, m_rectangle.z, m_rectangle.w);
        m_view = glm::translate(glm::mat4(1.0f), glm::vec3(m_offset, 0.0f));
        m_transform = m_projection * m_view;

        m_rebuild = false;
    }
}

const glm::vec2& ScreenSpace::GetSourceSize() const
{
    return m_sourceSize;
}

const glm::vec2& ScreenSpace::GetTargetSize() const
{
    return m_targetSize;
}

const glm::vec4& ScreenSpace::GetRectangle() const
{
    Rebuild();
    return m_rectangle;
}

const glm::vec2& ScreenSpace::GetOffset() const
{
    Rebuild();
    return m_offset;
}

const glm::mat4& ScreenSpace::GetProjection() const
{
    Rebuild();
    return m_projection;
}

const glm::mat4& ScreenSpace::GetView() const
{
    Rebuild();
    return m_view;
}

const glm::mat4& ScreenSpace::GetTransform() const
{
    Rebuild();
    return m_transform;
}
