#include "Precompiled.hpp"
#include "SpriteSheet.hpp"
#include "Texture.hpp"
#include "Lua/State.hpp"
#include "System/ResourceManager.hpp"
using namespace Graphics;

namespace
{
    // Log messages.
    #define LogLoadError(filename) "Failed to load a sprite sheet from \"" << filename << "\" file! "

    // Invalid sprite rectangle.
    const glm::vec4 InvalidSprite(0.0f, 0.0f, 0.0f, 0.0f);
}

SpriteSheet::SpriteSheet(System::ResourceManager* resourceManager) :
    Resource(resourceManager)
{
}

SpriteSheet::~SpriteSheet()
{
}

void SpriteSheet::Cleanup()
{
    // Reset texture reference.
    m_texture = nullptr;

    // Clear the list of sprites.
    Utility::ClearContainer(m_sprites);
}

bool SpriteSheet::Load(std::string filename)
{
    // Restore instance to it's original state.
    this->Cleanup();

    // Setup the cleanup scope guard.
    bool success = false;

    SCOPE_GUARD_IF(!success, this->Cleanup());

    // Get the resource manager.
    System::ResourceManager* resourceManager = this->GetResourceManager();

    if(resourceManager == nullptr)
    {
        Log() << LogLoadError(filename) << "Context is missing ResourceManager instance.";
        return false;
    }

    // Load sprite sheet file.
    Lua::State lua;

    if(!lua.Load(filename))
    {
        Log() << LogLoadError(filename) << "Couldn't load the file.";
        return false;
    }

    // Get the global table.
    lua_getglobal(lua, "SpriteSheet");

    if(!lua_istable(lua, -1))
    {
        Log() << LogLoadError(filename) << "Table \"SpriteSheet\" is missing or invalid.";
        return false;
    }

    // Load the sprite texture.
    lua_getfield(lua, -1, "Texture");

    if(!lua_isstring(lua, -1))
    {
        Log() << LogLoadError(filename) << "Field \"SpriteSheet.Texture\" is missing or invalid.";
        return false;
    }

    m_texture = resourceManager->Load<Texture>(lua_tostring(lua, -1));

    lua_pop(lua, 1);

    // Read the sprite table.
    lua_getfield(lua, -1, "Sprites");

    if(!lua_istable(lua, -1))
    {
        Log() << LogLoadError(filename) << "Field \"SpriteSheet.Sprites\" is missing or invalid.";
        return false;
    }

    // Iterate over the sprite table.
    for(lua_pushnil(lua); lua_next(lua, -2); lua_pop(lua, 1))
    {
        // Check if the key is a string.
        if(!lua_isstring(lua, -2))
        {
            Log() << LogLoadError(filename) << "One of \"SpriteSheet.Sprites\" keys is not a string.";
            return false;
        }

        // Read the rectangle.
        glm::vec4 rectangle;

        for(int i = 0; i < 4; ++i)
        {
            lua_pushinteger(lua, i + 1);
            lua_gettable(lua, -2);

            rectangle[i] = (float)lua_tonumber(lua, -1);

            lua_pop(lua, 1);
        }

        // Add sprite.
        if(!this->AddSprite(lua_tostring(lua, -2), rectangle))
        {
            Log() << LogLoadError(filename) << "Couldn't add a sprite.";
            return false;
        }
    }

    lua_pop(lua, 1);

    // Success!
    Log() << "Loaded a sprite sheet from \"" << filename << "\" file.";

    return success = true;
}

void SpriteSheet::SetTexture(TexturePtr texture)
{
    m_texture = texture;
}

const SpriteSheet::TexturePtr& SpriteSheet::GetTexture() const
{
    return m_texture;
}

bool SpriteSheet::AddSprite(std::string name, const glm::vec4& rectangle)
{
    if(name.empty())
        return false;

    // Add a sprite rectangle.
    auto result = m_sprites.emplace(name, rectangle);

    if(!result.second)
    {
        Log() << "Sprite with \"" << name << "\" name already exists within this sprite sheet!";
        return false;
    }

    return true;
}

const glm::vec4& SpriteSheet::GetSprite(std::string name) const
{
    if(name.empty())
        return InvalidSprite;

    // Find sprite rectangle by name,
    auto it = m_sprites.find(name);

    if(it == m_sprites.end())
        return InvalidSprite;

    return it->second;
}
