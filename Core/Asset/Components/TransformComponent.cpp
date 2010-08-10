#include "TransformComponent.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS( TransformComponent );

void TransformComponent::EnumerateClass( Reflect::Compositor< TransformComponent >& comp )
{
  comp.GetComposite().m_UIName = TXT( "Transform" );

  Reflect::Field* fieldTranslation = comp.AddField( &TransformComponent::m_Translation, "m_Translation" );
  Reflect::Field* fieldRotation = comp.AddField( &TransformComponent::m_Rotation, "m_Rotation" );
  Reflect::Field* fieldScale = comp.AddField( &TransformComponent::m_Scale, "m_Scale" );
}

Component::ComponentUsage TransformComponent::GetComponentUsage() const
{
    return Component::ComponentUsages::Overridable;
}


const Math::Vector3& TransformComponent::GetTranslation() const
{
    return m_Translation;
}

const Math::Vector3& TransformComponent::GetRotation() const
{
    return m_Rotation;
}

const Math::Vector3& TransformComponent::GetScale() const
{
    return m_Scale;
}

void TransformComponent::SetTranslation( const Math::Vector3& translation )
{
    m_Translation = translation;
}

void TransformComponent::SetRotation( const Math::Vector3& rotation )
{
    m_Rotation = rotation;
}

void TransformComponent::SetScale( const Math::Vector3& scale )
{
    m_Scale = scale;
}
