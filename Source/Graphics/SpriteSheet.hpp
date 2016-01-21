#pragma once

#include "Precompiled.hpp"
#include "System/Resource.hpp"

// Forward declarations.
namespace Graphics
{
    class Texture;
}

//
// Sprite Sheet
//
//  Loads a list of sprite definitions along with the texture.
//

namespace Graphics
{
    // Sprite sheet class.
    class SpriteSheet : public System::Resource
    {
    public:
        // Type declarations.
        typedef std::shared_ptr<const Texture> TexturePtr;
        typedef std::map<std::string, glm::vec4> SpriteList;

    public:
        SpriteSheet(System::ResourceManager* resourceManager);
        ~SpriteSheet();

        // Restores instance to it's original state.
        void Cleanup();

        // Loads the sprite sheet from a file.
        bool Load(std::string filename);

        // Sets the texture.
        void SetTexture(TexturePtr texture);

        // Gets the texture.
        const TexturePtr& GetTexture() const;

        // Adds a sprite.
        bool AddSprite(std::string name, const glm::vec4& rectangle);

        // Gets a sprite.
        const glm::vec4& GetSprite(std::string name) const;

    private:
        // Sprite sheet data.
        TexturePtr m_texture;
        SpriteList m_sprites;
    };
}
