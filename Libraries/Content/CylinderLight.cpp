#include "CylinderLight.h"
#include "ContentVisitor.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(CylinderLight)

void CylinderLight::EnumerateClass( Reflect::Compositor<CylinderLight>& comp )
{
  Reflect::Field* fieldLength = comp.AddField( &CylinderLight::m_Length, "m_Length" );
}

void CylinderLight::Host(ContentVisitor* visitor)
{
  visitor->VisitCylinderLight(this); 
}

void CylinderLight::CalculateSampleDensity()
{

  m_RegularDensity[0] = std::max<u32>( u32(m_Length * m_SamplesPerMeter), 2 );
  m_RegularDensity[1] = std::max<u32>( u32( 2 * Math::Pi * m_Radius * m_SamplesPerMeter ), 2 );

  m_LowDensity[0] =  std::max<u32>( u32( m_RegularDensity[0] * 0.5f), 2 );
  m_LowDensity[1] =  std::max<u32>( u32( m_RegularDensity[1] * 0.5f), 2 );
}