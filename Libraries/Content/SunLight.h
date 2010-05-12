#pragma once

#include "API.h"
#include "DirectionalLight.h"

namespace Content
{
  class CONTENT_API SunLight : public DirectionalLight
  {
  public:
    virtual void Host(ContentVisitor* visitor); 

    SunLight ()
      : DirectionalLight(),
      m_Haze( 0.0f ),
      m_RedBlueShift( 0.0f ),
      m_Saturation( 1.0f ),
      m_HorizonHeight( 0.0f ),
      m_HorizonBlur( 0.0f ),
      m_SunIntensity( 1.0f ),
      m_SkyIntensity( 1.0f )
      
      
    {
      m_GroundColor.Set( 0.2f, 0.2f, 0.2f );
    }

    SunLight (UniqueID::TUID &id)
      : DirectionalLight (id),
      m_Haze( 0.0f ),
      m_RedBlueShift( 0.0f ),
      m_Saturation( 1.0f ),
      m_HorizonHeight( 0.0f ),
      m_HorizonBlur( 0.0f ),
      m_SunIntensity( 1.0f ),
      m_SkyIntensity( 1.0f )
 
    {
      m_GroundColor.Set( 0.2f, 0.2f, 0.2f );
    }

    const static f32 MinHaze;
    const static f32 MaxHaze;

    const static f32 MinRedBlueShift;
    const static f32 MaxRedBlueShift;

    const static f32 MinSaturation;
    const static f32 MaxSaturation;

    const static f32 MinHorizonHeight;
    const static f32 MaxHorizonHeight;

    const static f32 MinHorizonBlur;
    const static f32 MaxHorizonBlur;
    
    f32 m_Haze;
    f32 m_RedBlueShift;
    f32 m_Saturation;
    f32 m_HorizonHeight;
    f32 m_HorizonBlur;
    f32 m_SunIntensity;
    f32 m_SkyIntensity;
    Math::Color3 m_GroundColor;


    REFLECT_DECLARE_CLASS(SunLight, DirectionalLight);

    static void EnumerateClass( Reflect::Compositor<SunLight>& comp );
  };

  typedef Nocturnal::SmartPtr<SunLight> SunLightPtr;
}