#pragma once

#include "Precompiled.hpp"
#include "Game/Component.hpp"

// Forward declarations.
namespace Graphics
{
    class Texture;
}

//
// Render Component
//

namespace Game
{
    namespace Components
    {
        // Forward declarations.
        class Transform;

        // Render component class.
        class Render : public Component
        {
        public:
            // Type declarations.
            typedef std::shared_ptr<const Graphics::Texture> TexturePtr;

        public:
            Render();
            ~Render();

            // Calculates the final color.
            glm::vec4 CalculateColor() const;

            // Sets the offset.
            void SetOffset(const glm::vec2& offset);

            // Sets the texture.
            void SetTexture(TexturePtr texture);
            void SetTexture(TexturePtr texture, const glm::vec4& rectangle);

            // Sets the rectangle.
            void SetRectangle(const glm::vec4& rectangle);

            // Set the diffuse color.
            void SetDiffuseColor(const glm::vec4& color);

            // Set the emissive color.
            void SetEmissiveColor(const glm::vec4& color);

            // Set the emissive power.
            void SetEmissivePower(float power);

            // Sets transparency state.
            void SetTransparent(bool transparent);

            // Gets the offset.
            const glm::vec2& GetOffset() const;

            // Gets the texture.
            const TexturePtr& GetTexture() const;

            // Gets the rectangle.
            const glm::vec4& GetRectangle() const;

            // Gets the diffuse color.
            const glm::vec4& GetDiffuseColor() const;

            // Gets the emissive color.
            const glm::vec4& GetEmissiveColor() const;

            // Gets the emissive power.
            float GetEmissivePower() const;

            // Checks if is transparent.
            bool IsTransparent() const;

            // Gets the transform component.
            Transform* GetTransform();

        protected:
            // Finalizes the render component.
            bool Finalize(EntityHandle self, const Context& context) override;

        private:
            // Texture resource.
            TexturePtr m_texture;
            glm::vec4 m_rectangle;

            // Render parameters.
            glm::vec2 m_offset;
            glm::vec4 m_diffuseColor;
            glm::vec4 m_emissiveColor;
            float m_emissivePower;
            bool m_transparent;

            // Entity components.
            Transform* m_transform;
        };
    }
}
