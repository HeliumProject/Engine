#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Content/Nodes/Instance/Instance.h"
#include "Pipeline/Content/ParametricKey/ParametricColorKey.h"
#include "Pipeline/Content/ParametricKey/ParametricIntensityKey.h"

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
      info->AddElement(Disabled ,       TXT( "Disabled" ) );
      info->AddElement(Vortex ,         TXT( "Vortex" ) );
      info->AddElement(VortexNoTrail ,  TXT( "Vortex No Trail" ) );
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
      info->AddElement(Point, TXT( "Point" ) );
      info->AddElement(Spot, TXT( "Spot" ) );
      info->AddElement(Directional, TXT( "Directional" ) );
      info->AddElement(LightingVolume, TXT( "LightingVolume" ) );
      info->AddElement(LightingEnvironment, TXT( "LightingEnvironment" ) );
      info->AddElement(Sun, TXT( "Sun" ) );
      info->AddElement(Portal, TXT( "Portal" ) );
      info->AddElement(AmbientDirectionals, TXT( "AmbientDirectionals" ) );
      info->AddElement(CubeMapProbe, TXT( "CubeMapProbe" ) );
      info->AddElement(Ambient, TXT( "Ambient" ) );
      info->AddElement(AmbientVolume, TXT( "AmbientVolume" ) );
      info->AddElement(SunShadow, TXT( "SunShadow" ) );
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
      info->AddElement(Baked, TXT( "Baked" ) );
      info->AddElement(RealTime, TXT( "RealTime" ) );
      info->AddElement(LightmapOnly, TXT( "LightmapOnly" ) );
    }
  };
  typedef LightRenderTypes::LightRenderType LightRenderType;

  class PIPELINE_API Light NOC_ABSTRACT : public Instance
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

    V_ParametricColorKeyPtr m_AnimationColor;

    V_ParametricColorKeyPtr m_AnimationIntensity;

    // real-time options
    float m_DrawDist;
    bool m_KillIfInactive;
    bool m_AllowOversized;
    bool m_RandomAnimOffset;
    bool m_DoVisibilityRayTest;    //Collision detection to determine visibility (Directional lights only)

    Light ( const Nocturnal::TUID& id = Nocturnal::TUID::Generate() )
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