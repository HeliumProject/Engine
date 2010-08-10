#include "ParametricIntensityKey.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS( ParametricIntensityKey )

void ParametricIntensityKey::EnumerateClass( Reflect::Compositor<ParametricIntensityKey>& comp )
{
  Reflect::Field* fieldIntensity = comp.AddField( &ParametricIntensityKey::m_Intensity, "m_Intensity" );
}


ParametricIntensityKey::ParametricIntensityKey()
: m_Intensity( 0 )
{
}

ParametricIntensityKey::~ParametricIntensityKey()
{
}

Math::Color3 ParametricIntensityKey::GetColor() const
{
  return Math::Color3( m_Intensity, m_Intensity, m_Intensity );
}

void ParametricIntensityKey::SetColor( const Math::Color3& color )
{
  u8 intensity = ( color.r + color.g + color.b ) / 3;
  if ( intensity != m_Intensity )
  {
    m_Intensity = intensity;
    
    RaiseChanged( GetClass()->FindField( &ParametricIntensityKey::m_Intensity ) );
  }
}