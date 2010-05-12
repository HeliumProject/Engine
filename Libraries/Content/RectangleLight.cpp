#include "RectangleLight.h"
#include "ContentVisitor.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(RectangleLight)

void RectangleLight::EnumerateClass( Reflect::Compositor<RectangleLight>& comp )
{
  Reflect::Field* fieldDimension = comp.AddField( &RectangleLight::m_Dimensions, "m_Dimensions" );
}


void RectangleLight::Host(ContentVisitor* visitor)
{
  visitor->VisitRectangleLight(this); 
}

void RectangleLight::CalculateSampleDensity()
{
  m_RegularDensity[0] = std::max<u32>( u32(m_Dimensions.x * m_SamplesPerMeter), 2 );
  m_RegularDensity[1] = std::max<u32>( u32(m_Dimensions.y * m_SamplesPerMeter), 2 );

  m_LowDensity[0] =  std::max<u32>( u32(m_RegularDensity[0] * 0.5f), 2);
  m_LowDensity[1] =  std::max<u32>( u32(m_RegularDensity[1] * 0.5f), 2);
}