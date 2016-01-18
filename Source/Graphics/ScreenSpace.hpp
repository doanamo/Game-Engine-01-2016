#pragma once

#include "Precompiled.hpp"

//
// Screen Space
//
//  Creates a screen space transform with an orthogonal projection.
//  Can be used for interface drawing or two dimensional view.
//
//  Requires source size (usually the frame buffer size) and target size to be
//  specified. The target will be enclosed inside the source, it will also be
//  scaled if the source size becomes smaller. The important thing to point out
//  is that the specified target, even when scaled, will always preserves it's
//  aspect ratio and coordinates.
//
//  Defining screen space coordinates:
//      Graphics::ScreenSpace screenSpace;
//      screenSpace.SetSourceSize(windowWidth, windowHeight);
//      screenSpace.SetTargetSize(1024, 576);
//      
//      glm::mat4 transform = screenSpace.GetTransform();
//

namespace Graphics
{
    // Screen space class.
    class ScreenSpace
    {
    public:
        ScreenSpace();
        ~ScreenSpace();

        // Restores instance to it's original state.
        void Cleanup();

        // Sets the source size.
        // Needs to be updated evey time source resizes.
        void SetSourceSize(int width, int height);

        // Sets the target size.
        void SetTargetSize(float width, float height);

        // Sets the target size to source size while preserving certain aspect ratio.
        // Aspect is equal to horizontal width divided by vertical height.
        void SetTargetSizeAspect(float aspect);

        // Gets the source size.
        const glm::vec2& GetSourceSize() const;

        // Gets the target size.
        const glm::vec2& GetTargetSize() const;

        // Gets the source rectangle.
        // Returns a [left, right, bottom, top] vector.
        const glm::vec4& GetRectangle() const;

        // Gets the offset from the center to bottom left corner of the target.
        // Can be used to move the origin to the center.
        const glm::vec2& GetOffset() const;

        // Gets the projection matrix.
        const glm::mat4& GetProjection() const;

        // Gets the view matrix.
        const glm::mat4& GetView() const;

        // Gets the combined projection and view matrices.
        const glm::mat4& GetTransform() const;

    private:
        glm::vec2 m_sourceSize;

        mutable glm::vec2 m_targetSize;
        float m_targetAspect;

        mutable glm::vec4 m_rectangle;
        mutable glm::vec2 m_offset;

        mutable glm::mat4 m_projection;
        mutable glm::mat4 m_view;
        mutable glm::mat4 m_transform;

   private:
        void Rebuild() const;

        mutable bool m_rebuild;
        mutable bool m_rebuildTarget;
    };
}
