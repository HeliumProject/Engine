#include "ParametricColorKey.h"

using namespace Content;

REFLECT_DEFINE_CLASS( ParametricColorKey )

void ParametricColorKey::EnumerateClass( Reflect::Compositor<ParametricColorKey>& comp )
{
  Reflect::Field* fieldColor = comp.AddField( &ParametricColorKey::m_Color, "m_Color" );
}


ParametricColorKey::ParametricColorKey()
{
}

ParametricColorKey::~ParametricColorKey()
{
}

Math::Color3 ParametricColorKey::GetColor() const
{
  return m_Color;
}

void ParametricColorKey::SetColor( const Math::Color3& color )
{
  if ( m_Color != color )
  {
    m_Color = color;

    RaiseChanged( GetClass()->FindField( &ParametricColorKey::m_Color ) );
  }
}