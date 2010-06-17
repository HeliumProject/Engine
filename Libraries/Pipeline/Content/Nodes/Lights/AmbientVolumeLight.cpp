#include "AmbientVolumeLight.h"
#include "Pipeline/Content/ContentVisitor.h" 

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(AmbientVolumeLight)

void AmbientVolumeLight::EnumerateClass( Reflect::Compositor<AmbientVolumeLight>& comp )
{
  Reflect::Field* fieldInnerRadius = comp.AddField( &AmbientVolumeLight::m_InnerRadius, "m_InnerRadius" );
  Reflect::Field* fieldOuterRadius = comp.AddField( &AmbientVolumeLight::m_OuterRadius, "m_OuterRadius" );
} 


void AmbientVolumeLight::Host(ContentVisitor* visitor)
{
  visitor->VisitAmbientVolumeLight(this); 
}