#pragma once
#include "API.h"
#include "Light.h"

namespace Content
{
  class CONTENT_API SpotLight : public Light
  {
  public:

    virtual void Host(ContentVisitor* visitor); 

    float m_InnerConeAngle;    
    float m_OuterConeAngle;

    // real-time options
    bool m_ShadowMapHiRes;
    
    bool  m_GodRayEnabled;
    float m_GodRayOpacity;
    
    float m_GodRayDensity;
    float m_GodRayQuality;
    float m_GodRayFadeNear;
    float m_GodRayFadeFar;

    float m_GodRayClipPlanePhiOffset;
    float m_GodRayClipPlaneOffset;

    u8 m_OffsetFactor;
    u8 m_OffsetUnits;

    SpotLight()
      : m_InnerRadius( 1.0f )
      , m_OuterRadius( 1.0f )
      , m_InnerConeAngle( Math::HalfPi * 0.5f )
      , m_OuterConeAngle( Math::HalfPi * 0.5f )
      , m_ShadowMapHiRes( false )
      , m_GodRayEnabled( false )
      , m_GodRayOpacity( 0.5f )
      , m_OffsetFactor( 96 )
      , m_OffsetUnits( 100 )
      , m_GodRayDensity(0.85f)
      , m_GodRayFadeNear(2.0f)
      , m_GodRayFadeFar(75.0f)
      , m_GodRayQuality(0.15f)
      , m_GodRayClipPlanePhiOffset(0.0f)
      , m_GodRayClipPlaneOffset(0.0f)
    {}

    SpotLight (Nocturnal::UID::TUID &id)
      : Light (id)
      , m_InnerRadius( 1.0f )
      , m_OuterRadius( 1.0f )
      , m_InnerConeAngle( Math::HalfPi * 0.5f )
      , m_OuterConeAngle( Math::HalfPi * 0.5f )
      , m_ShadowMapHiRes( false )
      , m_GodRayEnabled( false )
      , m_GodRayOpacity( 0.5f )
      , m_OffsetFactor( 96 )
      , m_OffsetUnits( 100 )
      , m_GodRayDensity(0.85f)
      , m_GodRayFadeNear(2.0f)
      , m_GodRayFadeFar(75.0f)
      , m_GodRayQuality(0.15f)
      , m_GodRayClipPlanePhiOffset(0.0f)
      , m_GodRayClipPlaneOffset(0.0f)
    {

    }

    const Math::Matrix4 GetGodRayClipPlaneTransform() const;
    const Math::Vector4 GetGodRayClipPlane()          const;

    REFLECT_DECLARE_CLASS(SpotLight, Light);

    static void EnumerateClass( Reflect::Compositor<SpotLight>& comp );
    static float GetEulerAngleLimit();

    float GetPhysicalBounds( float threshold = 0.1f ) const;
    float GetOuterRadius() const;
    float GetInnerRadius() const;

    void SetOuterRadius( float radius );
    void SetInnerRadius( float radius );

  private:
    float m_InnerRadius;
    float m_OuterRadius;
  };

  typedef Nocturnal::SmartPtr<SpotLight> SpotLightPtr;
  typedef std::vector<SpotLightPtr> V_SpotLight;
}