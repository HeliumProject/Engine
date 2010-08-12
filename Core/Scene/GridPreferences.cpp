/*#include "Precompile.h"*/
#include "GridPreferences.h"

#include "Foundation/Reflect/Serializer.h"
#include "Foundation/Reflect/Serializers.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_CLASS( GridPreferences );

GridPreferences::GridPreferences( const tstring& version,
                                  GridUnit units,
                                  u32 width,
                                  u32 length,
                                  f32 majorStep,
                                  f32 minorStep,
                                  Math::Color3 axisColor,
                                  Math::Color3 majorColor,
                                  Math::Color3 minorColor )
: m_Units( units )
, m_PreviousUnits( units )
, m_Width( width )
, m_Length( length )
, m_MajorStep( majorStep )
, m_MinorStep( minorStep )
, m_AxisColor( axisColor )
, m_MajorColor( majorColor )
, m_MinorColor( minorColor )
{
  AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &GridPreferences::OnChanged ) );
}

GridPreferences::~GridPreferences()
{
  RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &GridPreferences::OnChanged ) );
}

void GridPreferences::PostDeserialize()
{
  m_PreviousUnits = m_Units;
}

u32 GridPreferences::GetWidth()
{
  return m_Width;
}

u32 GridPreferences::GetLength()
{
  return m_Length;
}

f32 GridPreferences::GetMajorStep()
{
  return ConvertUnits( m_MajorStep, m_Units, GridUnits::Meters );
}

f32 GridPreferences::GetMinorStep()
{
  return ConvertUnits( m_MinorStep, m_Units, GridUnits::Meters );
}

const Math::Color3& GridPreferences::GetAxisColor()
{
  return m_AxisColor;
}

const Math::Color3& GridPreferences::GetMajorColor()
{
  return m_MajorColor;
}

const Math::Color3& GridPreferences::GetMinorColor()
{
  return m_MinorColor;
}

void GridPreferences::OnChanged( const Reflect::ElementChangeArgs& args )
{
  if ( m_Units == m_PreviousUnits )
  {
    return;
  }
  
  m_MajorStep = ConvertUnits( m_MajorStep, m_PreviousUnits, m_Units );
  m_MinorStep = ConvertUnits( m_MinorStep, m_PreviousUnits, m_Units );
  m_PreviousUnits = m_Units;

  RaiseChanged();
}

f32 GridPreferences::GetConversionFactor( GridUnit units )
{
  switch ( units )
  {
    case GridUnits::Centimeters:
      return 0.01f;
  }
  
  return 1.0f;
}

f32 GridPreferences::ConvertUnits( f32 sourceValue, GridUnit sourceUnits, GridUnit destinationUnits )
{
  f32 sourceConversion = GetConversionFactor( sourceUnits );
  f32 destinationConversion = GetConversionFactor( destinationUnits );
  return sourceValue * sourceConversion / destinationConversion;
}
