#include "Precompile.h"
#include "MiscSettings.h"

#include "Color.h"
#include "Foundation/Log.h"
#include "FileSystem/FileSystem.h"
#include "Finder/LunaSpecs.h"
#include "Math/Utils.h"

#include <tinyxml.h>

using namespace Luna;

REFLECT_DEFINE_CLASS( ScaleColorModeValue );
REFLECT_DEFINE_CLASS( MiscSettings );

bool ColorModeSort( const ScaleColorModeValue* left, const ScaleColorModeValue* right )
{
  return left->GetPercentage() < right->GetPercentage();
}

void ScaleColorModeValue::EnumerateClass( Reflect::Compositor<ScaleColorModeValue>& comp )
{
  comp.AddField( &ScaleColorModeValue::m_Percentage, "Percentage", Reflect::FieldFlags::Force  );
  comp.AddField( &ScaleColorModeValue::m_Red, "Red", Reflect::FieldFlags::Force );
  comp.AddField( &ScaleColorModeValue::m_Green, "Green", Reflect::FieldFlags::Force );
  comp.AddField( &ScaleColorModeValue::m_Blue, "Blue", Reflect::FieldFlags::Force );
}

void MiscSettings::EnumerateClass( Reflect::Compositor<MiscSettings>& comp )
{
  comp.AddField( &MiscSettings::m_ColorModeSettings, "Scale Color Mode Settings", Reflect::FieldFlags::Force );
}

void MiscSettings::LoadFromFile( MiscSettingsPtr& miscSettings )
{
#pragma TODO("reimplement to use a valid config folder" )
    std::string miscSettingsFile = ""; //FinderSpecs::Luna::MISC_SETTINGS.GetFile( FinderSpecs::Luna::GLOBAL_CONFIG_FOLDER );

  MiscSettingsPtr miscSettingsPtr = NULL;
  
  if ( FileSystem::Exists( miscSettingsFile ) )
  {
    try
    {
      miscSettingsPtr = Reflect::Archive::FromFile<MiscSettings>( miscSettingsFile );
    }
    catch ( ... )
    {
    }
  }
  
  if ( miscSettingsPtr == NULL )
  {
    miscSettingsPtr = new MiscSettings();
    miscSettingsPtr->SetDefaults();
  }
  
  miscSettings = miscSettingsPtr;

  // miscSettingsPtr->SetDefaults();
  // Reflect::Archive::ToFile( miscSettings, miscSettingsFile );
}

void MiscSettings::SetDefaults()
{
  m_ColorModeSettings.clear();
  m_ColorModeSettings.push_back( new ScaleColorModeValue( 0.000f, 0x00, 0xFF, 0x00 ) );
  m_ColorModeSettings.push_back( new ScaleColorModeValue( 0.149f, 0xFF, 0xFF, 0x00 ) );
  m_ColorModeSettings.push_back( new ScaleColorModeValue( 0.299f, 0xFF, 0x00, 0x00 ) );
}

void MiscSettings::PreSerialize()
{
  std::sort( m_ColorModeSettings.begin(), m_ColorModeSettings.end(), ColorModeSort );
}

void MiscSettings::PostDeserialize()
{
  std::sort( m_ColorModeSettings.begin(), m_ColorModeSettings.end(), ColorModeSort );
}

D3DCOLORVALUE MiscSettings::GetScaleColorLowerBound( f32 percentage, f32* bound ) const
{
  f32 b = 0.0f;
  D3DCOLORVALUE returnVal = Color::ColorToColorValue( 0xFF, 0x00, 0x00, 0x00 );

  V_ScaleColorModeValue::const_iterator end = m_ColorModeSettings.end();
  V_ScaleColorModeValue::const_iterator lowerBound = end;
  for ( V_ScaleColorModeValue::const_iterator itr = m_ColorModeSettings.begin(); itr != end; ++itr )
  {
    if ( (*itr)->GetPercentage() > percentage )
    {
      break;
    }

    lowerBound = itr;
  }

  if ( lowerBound == end )
  {
    lowerBound = m_ColorModeSettings.begin();
  }

  if ( lowerBound != end )
  {
    b = (*lowerBound)->GetPercentage();
    returnVal = Color::ColorToColorValue( 0xFF, (*lowerBound)->GetRed(), (*lowerBound)->GetGreen(), (*lowerBound)->GetBlue() );
  }

  if ( bound )
  {
    *bound = b;
  }

  return returnVal;
}

D3DCOLORVALUE MiscSettings::GetScaleColorUpperBound( f32 percentage, f32* bound ) const
{
  f32 b = 0.0f;
  D3DCOLORVALUE returnVal = Color::ColorToColorValue( 0xFF, 0x00, 0x00, 0x00 );

  V_ScaleColorModeValue::const_reverse_iterator end = m_ColorModeSettings.rend();
  V_ScaleColorModeValue::const_reverse_iterator upperBound = end;
  for ( V_ScaleColorModeValue::const_reverse_iterator itr = m_ColorModeSettings.rbegin(); itr != end; ++itr )
  {
    if ( (*itr)->GetPercentage() < percentage )
    {
      break;
    }

    upperBound = itr;
  }

  if ( upperBound == end )
  {
    upperBound = m_ColorModeSettings.rbegin();
  }

  if ( upperBound != end )
  {
    b = (*upperBound)->GetPercentage();
    returnVal = Color::ColorToColorValue( 0xFF, (*upperBound)->GetRed(), (*upperBound)->GetGreen(), (*upperBound)->GetBlue() );
  }

  if ( bound )
  {
    *bound = b;
  }

  return returnVal;
}

D3DCOLORVALUE MiscSettings::GetScaleColor( f32 percentage ) const
{
  return GetScaleColorLowerBound( percentage );
}

D3DCOLORVALUE MiscSettings::GetScaleColorGradient( f32 percentage ) const
{
  f32 lowerBound = 0.0f;
  D3DCOLORVALUE lowerColor = GetScaleColorLowerBound( percentage, &lowerBound );
  if ( percentage < lowerBound )
  {
    return lowerColor;
  }

  f32 upperBound = 0.0f;
  D3DCOLORVALUE upperColor = GetScaleColorUpperBound( percentage, &upperBound );
  if ( percentage > upperBound )
  {
    return upperColor;
  }

  f32 delta = upperBound - lowerBound;
  if ( delta < 0.001f )
  {
    return lowerColor;
  }
  
  return Color::ColorToColorValue( Color::BlendColor( Color::ColorValueToColor( lowerColor ), Color::ColorValueToColor( upperColor ), ( percentage - lowerBound ) / delta ) );
}
