#pragma once

#include "API.h"
#include "Instance.h"
#include "AreaLight.h"
#include "ParametricColorKey.h"
#include "ParametricIntensityKey.h"

namespace Content
{
  namespace LensFlareTypes
  {
    enum LensFlareType
    {
      Disabled,
      Vortex,
      VortexNoTrail,
    };

    static void LensFlareTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Disabled ,       "Disabled");
      info->AddElement(Vortex ,         "Vortex");
      info->AddElement(VortexNoTrail ,  "Vortex No Trail");
    }
  }
  typedef LensFlareTypes::LensFlareType LensFlareType;

  namespace LightTypes
  {
    enum LightType
    {
      Point,
      Spot,
      Directional,
      LightingVolume,
      LightingEnvironment,
      Sun,
      Portal,
      AmbientDirectionals,
      CubeMapProbe,
      Ambient,
      AmbientVolume,
      SunShadow
    };
    static void LightTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Point, "Point");
      info->AddElement(Spot, "Spot");
      info->AddElement(Directional, "Directional");
      info->AddElement(LightingVolume, "LightingVolume");
      info->AddElement(LightingEnvironment, "LightingEnvironment");
      info->AddElement(Sun, "Sun");
      info->AddElement(Portal, "Portal");
      info->AddElement(AmbientDirectionals, "AmbientDirectionals");
      info->AddElement(CubeMapProbe, "CubeMapProbe");
      info->AddElement(Ambient, "Ambient");
      info->AddElement(AmbientVolume, "AmbientVolume");
      info->AddElement(SunShadow, "SunShadow");
    }
  };
  typedef LightTypes::LightType LightType;

  namespace LightRenderTypes
  {
    enum LightRenderType
    {
      Baked,
      RealTime,
      LightmapOnly,
    };
    static void LightRenderTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Baked, "Baked");
      info->AddElement(RealTime, "RealTime");
      info->AddElement(LightmapOnly, "LightmapOnly");
    }
  };
  typedef LightRenderTypes::LightRenderType LightRenderType;

  class CONTENT_API Light NOC_ABSTRACT : public Instance
  {
  public:
    virtual void Host(ContentVisitor* visitor);

    bool m_Attenuate;

    bool m_CastShadows;

    bool m_EmitPhotons;

    bool m_PhysicalLight;

    u32 m_NumPhotons;

    // how much of the light "penetrates" any intersected object ( 0.0f == none <--> 1.0f == all )
    f32  m_Factor;

    f32  m_AnimationDuration;

    Math::HDRColor3 m_Color;

    Math::HDRColor3 m_PhotonEnergy;

    LightRenderType m_RenderType;
    LensFlareType   m_FlareType;

    AreaLightPtr m_AreaLight;

    V_ParametricColorKeyPtr m_AnimationColor;

    V_ParametricColorKeyPtr m_AnimationIntensity;

    // real-time options
    float m_DrawDist;
    bool m_KillIfInactive;
    bool m_AllowOversized;
    bool m_RandomAnimOffset;
    bool m_DoVisibilityRayTest;    //Collision detection to determine visibility (Directional lights only)

    Light ( const Nocturnal::UID::TUID& id = Nocturnal::UID::TUID::Generate() )
      : Instance( id )
      , m_Attenuate( true )
      , m_CastShadows( true )
      , m_EmitPhotons( false )
      , m_PhysicalLight( false )
      , m_NumPhotons( 100000 )
      , m_Factor( 0.0f )
      , m_AnimationDuration( 0.0f )
      , m_Color(1.0f,1.0f,1.0f)
      , m_RenderType( LightRenderTypes::Baked )
      , m_FlareType( LensFlareTypes::Disabled )
      , m_AreaLight( NULL )
      , m_KillIfInactive( false )
      , m_AllowOversized( true )
      , m_DrawDist( 70.0f )
      , m_RandomAnimOffset( true )
      , m_DoVisibilityRayTest( true )
    {

    }

    REFLECT_DECLARE_ABSTRACT(Light, Instance);

    static void EnumerateClass( Reflect::Compositor<Light>& comp );


    inline bool IsBakedLight()
    {
      //We are only interested in lights that have the baked property

      if( (m_RenderType == Content::LightRenderTypes::Baked) ||
        (m_RenderType == Content::LightRenderTypes::LightmapOnly ))
      {
        return true;
      }

      return false;
    }

  protected:

  };

  typedef Nocturnal::SmartPtr<Light> LightPtr;
  typedef std::vector<LightPtr> V_Light;
}