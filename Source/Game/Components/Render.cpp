#include "Precompiled.hpp"
#include "Render.hpp"
#include "Transform.hpp"
#include "Game/ComponentSystem.hpp"
#include "Graphics/Texture.hpp"
using namespace Game;
using namespace Components;

Render::Render() :
    m_offset(0.0f, 0.0f),
    m_rectangle(0.0f, 0.0f, 1.0f, 1.0f),
    m_diffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
    m_emissiveColor(1.0f, 1.0f, 1.0f, 1.0f),
    m_emissivePower(0.0f),
    m_transparent(true),
    m_transform(nullptr)
{
}

Render::~Render()
{
}

bool Render::Finalize(EntityHandle self, const Context& context)
{
    // Get required systems.
    ComponentSystem* componentSystem = context.Get<ComponentSystem>();
    if(componentSystem == nullptr) return false;

    // Get required components.
    m_transform = componentSystem->Lookup<Transform>(self);
    if(m_transform == nullptr) return false;

    return true;
}

void Render::SetOffset(const glm::vec2& offset)
{
    m_offset = offset;
}

glm::vec4 Render::CalculateColor() const
{
    return glm::mix(m_diffuseColor, m_emissiveColor, m_emissivePower);
}

void Render::SetTexture(TexturePtr texture)
{
    m_texture = texture;
    m_rectangle = glm::vec4(0.0f, 0.0f, texture->GetWidth(), texture->GetHeight());
}

void Render::SetTexture(TexturePtr texture, const glm::vec4& rectangle)
{
    m_texture = texture;
    m_rectangle = rectangle;
}

void Render::SetRectangle(const glm::vec4& rectangle)
{
    m_rectangle = rectangle;
}

void Render::SetDiffuseColor(const glm::vec4& color)
{
    m_diffuseColor = color;
}

void Render::SetEmissiveColor(const glm::vec4& color)
{
    m_emissiveColor = color;
}

void Render::SetEmissivePower(float power)
{
    m_emissivePower = power;
}

void Render::SetTransparent(bool transparent)
{
    m_transparent = transparent;
}

const glm::vec2& Render::GetOffset() const
{
    return m_offset;
}

const Render::TexturePtr& Render::GetTexture() const
{
    return m_texture;
}

const glm::vec4& Render::GetRectangle() const
{
    return m_rectangle;
}

const glm::vec4& Render::GetDiffuseColor() const
{
    return m_diffuseColor;
}

const glm::vec4& Render::GetEmissiveColor() const
{
    return m_emissiveColor;
}

float Render::GetEmissivePower() const
{
    return m_emissivePower;
}

bool Render::IsTransparent() const
{
    return m_transparent;
}

Transform* Render::GetTransform()
{
    return m_transform;
}
