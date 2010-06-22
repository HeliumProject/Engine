#include "AreaLight.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_ABSTRACT(AreaLight)

void AreaLight::EnumerateClass( Reflect::Compositor<AreaLight>& comp )
{
  Reflect::Field* fieldRegularDensity = comp.AddField( &AreaLight::m_RegularDensity, "m_RegularDensity" );
  Reflect::Field* fieldLowDensity = comp.AddField( &AreaLight::m_LowDensity, "m_LowDensity" );
  Reflect::Field* fieldSamplesPerMeter = comp.AddField( &AreaLight::m_SamplesPerMeter, "m_SamplesPerMeter" );
}