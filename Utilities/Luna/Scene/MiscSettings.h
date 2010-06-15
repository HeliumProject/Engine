#pragma once

#include "Luna/API.h"
#include "Reflect/Serializers.h"

namespace Luna
{
  class ScaleColorModeValue;
  class MiscSettings;

  typedef Nocturnal::SmartPtr<ScaleColorModeValue> ScaleColorModeValuePtr;
  typedef Nocturnal::SmartPtr<MiscSettings> MiscSettingsPtr;
  typedef std::vector<ScaleColorModeValuePtr> V_ScaleColorModeValue;



  class LUNA_CORE_API ScaleColorModeValue : public Reflect::ConcreteInheritor<ScaleColorModeValue, Reflect::Element>
  {
  public:
    ScaleColorModeValue( f32 percentage = 0.0f, u8 red = 0x00, u8 green = 0x00, u8 blue = 0x00 )
    : m_Percentage( percentage )
    , m_Red( red )
    , m_Green( green )
    , m_Blue( blue )
    {
    
    }
    
    bool operator<( const ScaleColorModeValue& right )
    {
      return m_Percentage < right.m_Percentage;
    }

    bool operator()( const ScaleColorModeValue& right )
    {
      return m_Percentage < right.m_Percentage;
    }

    static void EnumerateClass( Reflect::Compositor<ScaleColorModeValue>& comp );
    
    f32 GetPercentage() const
    {
      return m_Percentage;
    }

    u8 GetRed() const
    {
      return m_Red;
    }

    u8 GetGreen() const
    {
      return m_Green;
    }

    u8 GetBlue() const
    {
      return m_Blue;
    }

  protected:
    f32 m_Percentage;
    u8 m_Red;
    u8 m_Green;
    u8 m_Blue;
  };
  
  

  class LUNA_CORE_API MiscSettings : public Reflect::ConcreteInheritor<MiscSettings, Reflect::Element>
  {
  public:
    MiscSettings()
    {

    }
    
    static void EnumerateClass( Reflect::Compositor<MiscSettings>& comp );

    static void LoadFromFile( MiscSettingsPtr& miscSettings );
    
    void SetDefaults();

    virtual void PreSerialize() NOC_OVERRIDE;
    virtual void PostDeserialize() NOC_OVERRIDE;

    D3DCOLORVALUE GetScaleColorLowerBound( f32 percentage, f32* bound = NULL ) const;
    D3DCOLORVALUE GetScaleColorUpperBound( f32 percentage, f32* bound = NULL ) const;
    D3DCOLORVALUE GetScaleColor( f32 percentage ) const;
    D3DCOLORVALUE GetScaleColorGradient( f32 percentage ) const;
    
  protected:
    V_ScaleColorModeValue m_ColorModeSettings;
  };
}
