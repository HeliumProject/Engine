#include "ContentAmbientLight.h"
#include "Core/Content/ContentVisitor.h" 

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(AmbientLight)

void AmbientLight::EnumerateClass( Reflect::Compositor<AmbientLight>& comp )
{
  Reflect::Field* fieldOcclusionColor = comp.AddField( &AmbientLight::m_OcclusionColor, "m_OcclusionColor" );
  Reflect::Field* fieldSamples        = comp.AddField( &AmbientLight::m_Samples, "m_Samples" );
  Reflect::Field* fieldSampleDistance = comp.AddField( &AmbientLight::m_SampleDistance, "m_SampleDistance" );

  Reflect::Field* fieldSampleEnvironment = comp.AddField( &AmbientLight::m_SampleEnvironment, "m_SampleEnvironment" );
  Reflect::Field* fieldDoAmbientOcclusion = comp.AddField( &AmbientLight::m_DoAmbientOcclusion, "m_DoAmbientOcclusion" );
  Reflect::Field* fieldSpread = comp.AddField( &AmbientLight::m_Spread, "m_Spread" );

}


void AmbientLight::Host(ContentVisitor* visitor)
{
  visitor->VisitAmbientLight(this); 
}

