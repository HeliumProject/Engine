#include "stdafx.h"
#include "Key.h"
#include "Reflect/Serializers.h"

using namespace Inspect;

REFLECT_DEFINE_CLASS( Key )

///////////////////////////////////////////////////////////////////////////////
// Enumerate this class type for RTTI.
// 
void Key::EnumerateClass( Reflect::Compositor< Key >& comp )
{
  Reflect::Field* fieldColor = comp.AddField( &Key::m_Color, "m_Color" );
  Reflect::Field* fieldLoc = comp.AddField( &Key::m_Location, "m_Location" );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Key::Key()
: m_Color( 0, 0, 0 )
, m_Location( 0.0f )
{
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Key::Key( const Math::Color3& color, float location )
: m_Color( color )
, m_Location( location )
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the color of this key.
// 
Math::Color3 Key::GetColor() const
{
  return m_Color;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the location of this key, i.e. a value from 0 to 1 that indicates 
// where on the control this key should appear.  0 means the far left, 1 means
// the far right.
// 
float Key::GetLocation() const
{
  return m_Location;
}
