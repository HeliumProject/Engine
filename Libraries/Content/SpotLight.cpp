#include "SpotLight.h"
#include "ContentVisitor.h"
#include "Foundation/Math/EulerAngles.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(SpotLight)

void SpotLight::EnumerateClass( Reflect::Compositor<SpotLight>& comp )
{
  comp.AddField( &SpotLight::m_InnerRadius,                 "m_InnerRadius" );
  comp.AddField( &SpotLight::m_OuterRadius,                 "m_OuterRadius" );
  comp.AddField( &SpotLight::m_InnerConeAngle,              "m_InnerConeAngle" );
  comp.AddField( &SpotLight::m_OuterConeAngle,              "m_OuterConeAngle" );
  comp.AddField( &SpotLight::m_ShadowMapHiRes,              "m_ShadowMapHiRes" );
  comp.AddField( &SpotLight::m_GodRayEnabled,               "m_GodRayEnabled" );
  comp.AddField( &SpotLight::m_GodRayOpacity,               "m_GodRayOpacity" );
  comp.AddField( &SpotLight::m_GodRayDensity,               "m_GodRayDensity" );
  comp.AddField( &SpotLight::m_GodRayQuality,               "m_GodRayQuality" ); 
  comp.AddField( &SpotLight::m_GodRayFadeNear,              "m_GodRayFadeNear" );
  comp.AddField( &SpotLight::m_GodRayFadeFar,               "m_GodRayFadeFar" );
  comp.AddField( &SpotLight::m_GodRayClipPlanePhiOffset,    "m_GodRayClipPlanePhiOffset" ); 
  comp.AddField( &SpotLight::m_GodRayClipPlaneOffset,       "m_GodRayClipPlaneOffset" );
  comp.AddField( &SpotLight::m_OffsetFactor,                "m_OffsetFactor" );
  comp.AddField( &SpotLight::m_OffsetUnits,                 "m_OffsetUnits" );
}

///////////////////////////////////////////////////////////////////////////////
// GodRay clip-plane transform matrix
float SpotLight::GetEulerAngleLimit()
{
  const float cAngleInDegrees = 45.0f;
  return (cAngleInDegrees * 0.017453292519943295769236907684886f);
}

///////////////////////////////////////////////////////////////////////////////
// GodRay clip-plane transform matrix
const Math::Matrix4 SpotLight::GetGodRayClipPlaneTransform() const
{
  Math::Matrix4 finalTransform;

  if(m_GodRayEnabled)
  {
    const float  eulerAngleLimit  = GetEulerAngleLimit();
    float        eulerAngle       = m_GodRayClipPlanePhiOffset*eulerAngleLimit;

    //Local Transform
    Math::Matrix4 localTransform(Math::EulerAngles(eulerAngle, 0.0f, 0.0f));
    localTransform.t  = Math::Vector4(0.0f, 0.0f, m_GodRayClipPlaneOffset, 1.0f);

    //Final Transform (the end to a series of duh! comments)
    finalTransform = localTransform * m_GlobalTransform;
  }

  //Return the end result
  return finalTransform;
}

///////////////////////////////////////////////////////////////////////////////
// GodRay clip-plane
const Math::Vector4 SpotLight::GetGodRayClipPlane() const
{
  Math::Matrix4 transform   = GetGodRayClipPlaneTransform();
  Math::Vector4 pointA( 0.0f, 0.0f, 0.0f, 1.0f );
  Math::Vector4 pointB( 0.0f, 0.0f, 1.0f, 1.0f );
  Math::Vector4 plane;

  pointA  = transform*pointA;
  pointB  = transform*pointB;

  plane   = pointB - pointA;
  plane.w = 0.0f;

  plane.Normalize();
  plane.w = -plane.Dot(pointA);

  return plane;
}

void SpotLight::Host(ContentVisitor* visitor)
{
  visitor->VisitSpotLight(this); 
}

float SpotLight::GetPhysicalBounds( float threshold ) const
{
  f32 intensity = m_Color.s * 50000.0f;
  return ( sqrt(intensity) / sqrt( threshold ) * Math::CentimetersToMeters );
}

float SpotLight::GetOuterRadius() const
{
  if( !m_PhysicalLight || m_RenderType == LightRenderTypes::RealTime )
    return m_OuterRadius;
  else
    return GetPhysicalBounds();
}

float SpotLight::GetInnerRadius() const
{
  if( !m_PhysicalLight || m_RenderType == LightRenderTypes::RealTime )
    return m_InnerRadius;
  return 0.0f;
}

void SpotLight::SetOuterRadius( float radius ) 
{
  if( !m_PhysicalLight || m_RenderType == LightRenderTypes::RealTime )
    m_OuterRadius = radius;
}

void SpotLight::SetInnerRadius( float radius ) 
{
  if( !m_PhysicalLight || m_RenderType == LightRenderTypes::RealTime )
    m_InnerRadius = radius;
}