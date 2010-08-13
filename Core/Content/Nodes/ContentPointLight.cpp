#include "ContentPointLight.h"
#include "Core/Content/ContentVisitor.h" 

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(PointLight)

void PointLight::EnumerateClass( Reflect::Compositor<PointLight>& comp )
{
  Reflect::Field* fieldInnerRadius = comp.AddField( &PointLight::m_InnerRadius, "m_InnerRadius" );
  Reflect::Field* fieldOuterRadius = comp.AddField( &PointLight::m_OuterRadius, "m_OuterRadius" );
}


void PointLight::Host(ContentVisitor* visitor)
{
  visitor->VisitPointLight(this); 
}

float PointLight::GetPhysicalBounds( float threshold ) const
{
  f32 intensity = m_Color.s * 50000.0f;
  return ( sqrt(intensity) / sqrt( threshold ) * Math::CentimetersToMeters );
}

float PointLight::GetOuterRadius() const
{
  if( !m_PhysicalLight || m_RenderType == LightRenderTypes::RealTime )
    return m_OuterRadius;
  else
    return GetPhysicalBounds();
}

float PointLight::GetInnerRadius() const
{
  if( !m_PhysicalLight || m_RenderType == LightRenderTypes::RealTime )
    return m_InnerRadius;
  return 0.0f;
}

void PointLight::SetOuterRadius( float radius ) 
{
  if( !m_PhysicalLight || m_RenderType == LightRenderTypes::RealTime )
    m_OuterRadius = radius;
}

void PointLight::SetInnerRadius( float radius ) 
{
  if( !m_PhysicalLight || m_RenderType == LightRenderTypes::RealTime )
    m_InnerRadius = radius;
}