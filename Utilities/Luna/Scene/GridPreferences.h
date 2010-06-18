#pragma once

#include "Foundation/Math/Color3.h"
#include "Foundation/Reflect/Element.h"

namespace Luna
{
  namespace GridUnits
  {
    enum GridUnit
    {
      Meters,
      Centimeters,
    };

    static void GridUnitEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Meters, "Meters");
      info->AddElement(Centimeters, "Centimeters");
    }
  }
  
  typedef GridUnits::GridUnit GridUnit;

  class GridPreferences;
  typedef Nocturnal::SmartPtr< GridPreferences > GridPreferencesPtr;

  class GridPreferences : public Reflect::Element
  {
  private:
    GridUnit m_Units;
    GridUnit m_PreviousUnits;
    u32 m_Width;
    u32 m_Length;
    f32 m_MajorStep;
    f32 m_MinorStep;
    Math::Color3 m_AxisColor;
    Math::Color3 m_MajorColor;
    Math::Color3 m_MinorColor;
    
  public:
    GridPreferences( const std::string& version = "",
                     GridUnit units = GridUnits::Meters,
                     u32 width = 12,
                     u32 length = 12,
                     f32 majorStep = 5.0f,
                     f32 minorStep = 1.0f,
                     Math::Color3 axisColor = Math::Color3( Math::Vector3( 0.0f, 0.0f, 0.0f ) ),
                     Math::Color3 majorColor = Math::Color3( Math::Vector3( 0.5f, 0.5f, 0.5f ) ),
                     Math::Color3 minorColor = Math::Color3( Math::Vector3( 0.5f, 0.5f, 0.5f ) ) );
    
    u32 GetWidth();
    u32 GetLength();
    f32 GetMajorStep();
    f32 GetMinorStep();
    const Math::Color3& GetAxisColor();
    const Math::Color3& GetMajorColor();
    const Math::Color3& GetMinorColor();
    
    ~GridPreferences();
    
    virtual void PostDeserialize();

    REFLECT_DECLARE_CLASS( GridPreferences, Reflect::Element )

    static void EnumerateClass( Reflect::Compositor< GridPreferences >& comp );
    
  private:
    void OnChanged( const Reflect::ElementChangeArgs& args );

    static f32 GetConversionFactor( GridUnit units );
    static f32 ConvertUnits( f32 sourceValue, GridUnit sourceUnits, GridUnit destinationUnits );
  };
}
