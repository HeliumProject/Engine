#include "RadiusLight.h"
#include "ContentVisitor.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_ABSTRACT(RadiusLight)

void RadiusLight::EnumerateClass( Reflect::Compositor<RadiusLight>& comp )
{
  Reflect::Field* fieldRadius = comp.AddField( &RadiusLight::m_Radius, "m_Radius" );
}

void RadiusLight::CalculateSampleDensity()
{
  f32 surfaceArea = 4 * Math::Pi * m_Radius * m_Radius;

  u32 density = u32(sqrt(surfaceArea) * m_SamplesPerMeter);
  m_RegularDensity[0] = std::max<u32>( density, 2 );
  m_RegularDensity[1] = std::max<u32>( density, 2 );

  density = u32(density * 0.5f);
  m_LowDensity[0] =  std::max<u32>( density, 2 );
  m_LowDensity[1] =  std::max<u32>( density, 2 );
}